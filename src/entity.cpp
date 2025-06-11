#include "entity.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>

#include "game.hpp"

std::shared_ptr<sf::RectangleShape> Entity::getShape() { return shape; }

sf::Vector2f Entity::getPos() { return shape->getPosition(); }

void Entity::setPos(sf::Vector2f pos) { shape->setPosition(pos); }

void Entity::setEnergy(int newEnergy) { energy = newEnergy; }
int Entity::getEnergy() { return energy; }

sf::Vector2f Entity::getSize() { return shape->getSize(); }

void Entity::setChase(bool chase_) { chase = chase_; }
void Entity::printState() {
  switch (currentBehavior) {
    case IDLE:
      std::cout << "IDLE" << std::endl;
      break;

    case CHASING:
      std::cout << "CHASING" << std::endl;
      break;
    case FLEEING:
      std::cout << "FLEEING" << std::endl;
      break;
    case CRAVING:
      std::cout << "CRAVING" << std::endl;
      break;
    case DOWN_BAD:
      std::cout << "DOWN BAD" << std::endl;
      break;
    default:
      break;
  }
}
Entity::Entity(std::shared_ptr<sf::RectangleShape> shape_, Game& game_,
               std::string id_)
    : shape(shape_), game(game_), id(id_) {
  // Ensure shape is initialized
  if (!shape) {
    throw std::runtime_error("Shape is not initialized");
  }
  initRandVariables();
  calcKeyValues();

  health = baseHealth;
  getRandomPos(wanderRange);
}

Entity::Entity(std::shared_ptr<sf::RectangleShape> shape_, Game& game_,
               std::string id_, const Entity& parent1, const Entity& parent2)
    : shape(shape_), game(game_), id(id_) {
  // Ensure shape is initialized
  if (!shape) {
    throw std::runtime_error("Shape is not initialized");
  }
  inheritVariables(parent1, parent2);
  calcKeyValues();
  health = baseHealth;
  getRandomPos(wanderRange);
}
void Entity::inheritVariables(const Entity& parent1, const Entity& parent2) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<float> dist(0.f, 0.2f);  // Mean 0, Std Dev 0.2

  auto mixVariable = [&](float var1, float var2) {
    return var1 * 0.5 + var2 * 0.5 + dist(gen);
  };

  speed = mixVariable(parent1.speed, parent2.speed);
  agility = mixVariable(parent1.agility, parent2.agility);
  efficiency = mixVariable(parent1.efficiency, parent2.efficiency);
  acceleration = mixVariable(parent1.acceleration, parent2.acceleration);
  minDistance = mixVariable(parent1.minDistance, parent2.minDistance);
  wanderRange = mixVariable(parent1.wanderRange, parent2.wanderRange);
  damage = mixVariable(parent1.damage, parent2.damage);
  baseHealth = mixVariable(parent1.baseHealth, parent2.baseHealth);
  aggression = mixVariable(parent1.aggression, parent2.aggression);
}
void Entity::initRandVariables() {
  /*
  Independently randomize the variables of the entity
  speed - 1
  agility - 0.3
  efficiency - 0.5
  acceleration - 0.01
  minDistance - 140
  wanderRange - 300
  damage - 10
  basehealth - 100
  */
  speed = 0.3f + static_cast<float>(rand()) / RAND_MAX * (0.9f - 0.1f);
  agility = 0.3f + static_cast<float>(rand()) / RAND_MAX * (0.9f - 0.3f);
  efficiency = static_cast<float>(rand()) / RAND_MAX;
  acceleration = 0.01f * (0.5f + static_cast<float>(rand()) / RAND_MAX);
  minDistance =
      30.0f + static_cast<float>(rand()) / RAND_MAX *
                  (140.0f - 30.0f);  // MinDistance is between 30 and 140
  wanderRange = 300.0f * (0.5f + static_cast<float>(rand()) / RAND_MAX);
  damage = 10.0f * (0.5f + static_cast<float>(rand()) / RAND_MAX);
  baseHealth = 100.0f * (0.5f + static_cast<float>(rand()) / RAND_MAX);
  aggression = static_cast<float>(rand()) / RAND_MAX;
}

bool Entity::collisionCheck() {
  for (const auto& entity : game.getEntities()) {
    if (entity->getId() != id && entity->isAlive()) {
      sf::FloatRect entityBounds = entity->getShape()->getGlobalBounds();
      // Fix the empty statement that does nothing
      if (entityBounds.contains(nextpos)) {
        nextpos = getPos();
        return true;
      }
    }
  }
  return false;
}
bool Entity::isObstacleBetween(sf::Vector2f destination) {
  sf::Vector2f currentPos = shape->getPosition();
  sf::Vector2f direction = destination - currentPos;
  float length =
      std::sqrt(direction.x * direction.x + direction.y * direction.y);
  direction /= length;

  for (const auto& obstacle : game.getObstacles()) {
    sf::FloatRect obstacleBounds = obstacle->getShape()->getGlobalBounds();
    for (float i = 0; i < length; i += 1.0f) {
      sf::Vector2f point = currentPos + direction * i;
      if (obstacleBounds.contains(point)) {
        return true;
      }
    }
  }
  return false;
}
void Entity::updateVariables() {
  energy = std::max(0.f, energy - (calcMovementCost()));
  if (currentSpeed> 0.5*speed) {
    stamina = std::max(200.f, stamina - 2 * calcMovementCost());
  } else {
    stamina = maxStamina;
  }
  currentSpeed = std::max(
      speed * 0.2f, static_cast<float>(currentSpeed * (stamina * 0.0001)));
  if (energy < maxEnergy * 0.1) {
    getDamaged(2);
  }
}
void Entity::Update() {
  if (!alive) {  // check if alive
    if (game.getDeltaTime() - dTime > 3 && !compost) {
      Compost();  // if dead for long enough time, compost
    }
    return;
  }
  if (checkIfIdle() | collisionCheck()) {  // if idle, get a random destination.
    getRandomPos(wanderRange);
  }

  handleEntities();   // Handle interractions with other entities
  updateVariables();  // update the variables of this entity.

  if (!isObstacleBetween(destination)) {  // if no obstacle between this and the
                                          // destination, move toward.
    calcNextPos(destination);
    lastpos = shape->getPosition();
    if (collisionCheck()) {
      getRandomPos(wanderRange * 0.2);
    }
    // std::cout << energy << std::endl;
    shape->setPosition(nextpos);
  } else {
    getRandomPos(wanderRange);
  }
  // printState();
}

bool Entity::checkIfIdle() {
  sf::Vector2f currentPos = shape->getPosition();
  float distance = std::sqrt(std::pow(destination.x - currentPos.x, 2) +
                             std::pow(destination.y - currentPos.y, 2));
  if (distance < 0.5) {
    currentBehavior = IDLE;
    return true;
  } else {
    return false;
  }
}

void Entity::setWanderRange(int range_) { wanderRange = range_; }

void Entity::setSpeed(float speed_) { speed = speed_; }
int Entity::getHealth() { return health; }
float Entity::getEfficiency() { return efficiency; }

void Entity::setEfficiency(float efficiency_) { efficiency = efficiency_; }
void Entity::setKillerEfficiency(float efficiency_) {
  killerEfficiency = efficiency_;
}
void Entity::calcKeyValues() {
  senseFactor = minDistance;
  threatFactor = (baseHealth * 0.01 + speed + damage);
  shape->setSize(sf::Vector2f((float)threatFactor, (float)threatFactor));
}
void Entity::setAcceleration(float acceleration_) {
  acceleration = acceleration_;
}
unsigned int Entity::getThreat() { return threatFactor; }
void Entity::setThreat(unsigned int input) { threatFactor = input; }
float Entity::getAggression() { return aggression; }
void Entity::getRandomPos(int range) {
  currentSpeed *= agility;
  sf::Vector2f currentPos = shape->getPosition();
  float randomX =
      currentPos.x +
      static_cast<float>(rand() % static_cast<int>(wanderRange * 2)) -
      wanderRange;
  float randomY =
      currentPos.y +
      static_cast<float>(rand() % static_cast<int>(wanderRange * 2)) -
      wanderRange;

  auto dest = sf::Vector2f(randomX, randomY);
  destination = dest;
}

bool Entity::isAlive() { return alive; }

void Entity::handleEntities() {
  sf::Vector2f currentPos = shape->getPosition();
  bool tooClose = false;
  float nearestDistance = minDistance;

  for (const auto& entity : game.getEntities()) {
    if (entity->getId() != id) {
      sf::Vector2f otherPos = entity->getPos();
      sf::Vector2f diff = currentPos - otherPos;
      float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

      // CHASE logic
      if (distance < nearestDistance && entity->isAlive() &&
          currentBehavior != DOWN_BAD &&
          entity->getThreat() < threatFactor * aggression &&
          distance < minDistance && !isObstacleBetween(otherPos) &&
          stamina > maxStamina * 0.3) {
        currentBehavior = CHASING;  // Toggle chase state
        nearestDistance = distance;
        destination = otherPos;
        if (distance < damageDistance) {  // eat

          if (entity->getDamaged(damage)) {  // returns true if is killed
            energy += entity->getEnergy() * efficiency;
            if (energy > maxEnergy * 0.5) {
              currentBehavior = DOWN_BAD;
            }
            getRandomPos(wanderRange);
            entity->setKillerEfficiency(efficiency);
            return;
          }
        }
      }
      // FLEE logic
      else if (distance < minDistance && entity->isAlive() &&
               entity->getAggression() * entity->getThreat() >= getThreat()) {
        currentBehavior = FLEEING;  // Toggle flee state
        tooClose = true;
        sf::Vector2f unitVector = diff / distance;
        sf::Vector2f dest = currentPos + unitVector * 15.f;
        if (isObstacleBetween(dest)) {
          dest = sf::Vector2f(dest.x + static_cast<float>(rand() % 20 - 10),
                              dest.y + static_cast<float>(rand() % 20 - 10));
        } else {
          destination = dest;
        }
      } else {  // IF IDLE
        // only reset to IDLE if we're not already in FLEEING or CRAVING state
        if (currentBehavior != FLEEING && currentBehavior != CRAVING &&
            currentBehavior != DOWN_BAD) {
          currentBehavior = IDLE;
        }
      }
    }
  }

  // NUTRIENT SEEK logic
  if (efficiency < 0.3 && currentBehavior != FLEEING &&
      currentBehavior != DOWN_BAD) {
    float mindistance = 100;
    bool foundAccessibleNutrient = false;
    sf::Vector2f closestNutrientPos;

    for (const auto& nutrient : game.getNutrients()) {
      if (nutrient != nullptr) {
        sf::Vector2f otherPos = nutrient->getPos();
        sf::Vector2f diff = currentPos - otherPos;
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (distance <= minDistance * energy / maxEnergy * efficiency + 40 &&
            !isObstacleBetween(otherPos)) {
          mindistance = distance;
          closestNutrientPos = otherPos;
          foundAccessibleNutrient = true;

          if (distance < damageDistance) {
            energy += nutrient->getEnergy();  // eating adds energy
            game.removeNutrient(nutrient->getId());
            if (energy > maxEnergy * 0.5) {
              currentBehavior = DOWN_BAD;
            } else {
              currentBehavior = IDLE;
            }
            getRandomPos(wanderRange);  // Get a new random position
            return;                     // Exit the method since we're done
          }
        }
      }
    }

    // If we found an accessible nutrient, move toward it
    if (foundAccessibleNutrient) {
      currentBehavior = CRAVING;
      destination = closestNutrientPos;
    } else if (currentBehavior == CRAVING) {
      // If we were in CRAVING state but can't find nutrients anymore,
      // reset to IDLE and get a new random position
      currentBehavior = IDLE;
      getRandomPos(wanderRange);
    }
  }
  // DOWN BAD logic
  if (currentBehavior == DOWN_BAD && stamina > 0.3*maxStamina) {
    for (const auto& entity : game.getEntities()) {
      if (entity->getId() != id && entity->currentBehavior == DOWN_BAD) {
        sf::Vector2f otherPos = entity->getPos();
        sf::Vector2f diff = getPos() - otherPos;
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (distance < minDistance && !isObstacleBetween(otherPos)) {
          destination = otherPos;
          if (distance < damageDistance) {
            currentBehavior = IDLE;
            game.spawnEntity(std::make_unique<Entity>(*this), entity);

            energy -= 5000;
            stamina -= 3000;
            return;
          }
        }
      }
    }
  }
}

sf::Vector2f Entity::getDirection() {
  sf::Vector2f direction = nextpos - getPos();
  float length =
      std::sqrt(direction.x * direction.x + direction.y * direction.y);
  if (length != 0)
    return direction / length;
  else
    return sf::Vector2f(0, 0);
}
bool Entity::getDamaged(unsigned int damage_) {
  health -= damage_;
  if (health <= 0) {  // checks if dead
    setAcceleration(0);
    shape->setFillColor(sf::Color::Red);
    alive = false;
    dTime = game.getDeltaTime();
    return true;
  }
  return false;
}
bool Entity::hasComposted() { return compost; }
void Entity::Compost() {
  game.addNutrient(getPos(), game.getNutrients().size(),
                   energy * (1 - killerEfficiency));  // spawn a nutrient
  compost = true;
  toBeRemoved = true;  // mark for removal
}
// Calc the next position
void Entity::calcNextPos(sf::Vector2f destinaton) {
  sf::Vector2f currentPos = shape->getPosition();
  sf::Vector2f direction = destinaton - currentPos;
  float length =
      std::sqrt(direction.x * direction.x + direction.y * direction.y);
  if (length != 0) {
    direction.x /= length;
    direction.y /= length;
  }
  nextpos = currentPos + sf::Vector2f(direction.x * currentSpeed,
                                      direction.y * currentSpeed);
  if (currentSpeed + acceleration <= speed) {
    currentSpeed += acceleration;
  }
}

std::string Entity::getId() { return id; }

float Entity::calcMovementCost() {
  /*std::cout << "Current Speed: " << currentSpeed * 3
            << " (Multiplier: 3), "
               "Threat Factor: "
            << threatFactor * 0.1
            << " (Multiplier: 0.1), "
               "Sense Factor: "
            << senseFactor * 0.01
            << " (Multiplier: 0.01), "
               "Aggression: "
            << aggression * 3
            << " (Multiplier: 3), "
               "Acceleration: "
            << acceleration * 80
            << " (Multiplier: 80), "
               "Agility: "
            << agility * 3 << " (Multiplier: 3)" << std::endl;*/
  return (currentSpeed * 3 + threatFactor * threatFactor * 0.01 +
          senseFactor * 0.01 + aggression * 2 + acceleration * 80 +
          agility * 3) *
         0.1;
}
