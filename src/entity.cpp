#include "entity.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "game.hpp"

std::shared_ptr<sf::RectangleShape> Entity::getShape() { return shape; }

sf::Vector2f Entity::getPos() { return shape->getPosition(); }

void Entity::setPos(sf::Vector2f pos) { shape->setPosition(pos); }

sf::Vector2f Entity::getSize() { return shape->getSize(); }

void Entity::setChase(bool chase_) { chase = chase_; }

Entity::Entity(std::shared_ptr<sf::RectangleShape> shape_, Game& game_,
               unsigned int id_)
    : shape(shape_), game(game_), id(id_) {
  // Ensure shape is initialized
  if (!shape) {
    throw std::runtime_error("Shape is not initialized");
  }
  getRandomPos(wanderRange);
}

bool Entity::collisionCheck() {
  for (const auto& entity : game.getEntities()) {
    if (entity->getId() != id && entity->isAlive()) {
      sf::FloatRect entityBounds = entity->getShape()->getGlobalBounds();
      nextpos;
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
  if (energy < maxEnergy * 0.2) {
    stamina = std::max(200.f, stamina - 2 * calcMovementCost());
  } else {
    stamina = maxStamina;
  }
  currentSpeed = std::max(
      speed * 0.2f, static_cast<float>(currentSpeed * (stamina / 10000.f)));
  if (energy < maxEnergy * 0.1) {
    getDamaged(1);
  }
}
void Entity::Update() {
  if (!alive) {  // check if alive
    if (game.getDeltaTime() - dTime > 3 && !compost) {
      Compost();  // if dead for long enough time, compost
    }
    return;
  }
  if (checkIfIdle()) {  // if idle, get a random destination.
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
    //std::cout << energy << std::endl;
    shape->setPosition(nextpos);
  } else {
    getRandomPos(wanderRange);
  }
}

bool Entity::checkIfIdle() {
  sf::Vector2f currentPos = shape->getPosition();
  float distance = std::sqrt(std::pow(destination.x - currentPos.x, 2) +
                             std::pow(destination.y - currentPos.y, 2));
  return distance < 0.5;
}

void Entity::setWanderRange(int range_) { wanderRange = range_; }

void Entity::setSpeed(float speed_) { speed = speed_; }

void Entity::setAcceleration(float acceleration_) {
  acceleration = acceleration_;
}

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
  float nearestDistance = aggroDistance;  // Initialize the nearestDistance var

  for (const auto& entity :
       game.getEntities()) {      // Loop through all the entities
    if (entity->getId() != id) {  // Make sure that the entity is not this one.
      sf::Vector2f otherPos = entity->getPos();
      sf::Vector2f diff = currentPos - otherPos;
      float distance = sqrt(diff.x * diff.x + diff.y * diff.y);

      if (distance < nearestDistance && entity->isAlive() &&
          chase &&  // if this entity is agrressive, the other
                    // entity is alive and near enough, chase him
          distance < aggroDistance && !isObstacleBetween(otherPos)) {
        nearestDistance = distance;
        destination =
            otherPos;  // set the destination to be the other entitys pos
        if (distance < damageDistance) {     // check if close enough to damage.
          if (entity->getDamaged(damage)) {  // damages the targe; if the target
                                             // dies get energy
            energy += entity->getSize().x * entity->getSize().y * 100;
            getRandomPos(wanderRange);
          }
        }
      }

      else if (distance < minDistance && entity->isAlive() && !chase &&
               (1.0f +
                std::max(0.f, 0.1f *
                                  (diff.x * entity->getDirection().x +
                                   diff.y * entity->getDirection().y) /
                                  distance)) &&
               entity->chase  // Check if the other entity is chasing, and the
                              // min distance
      ) {
        tooClose = true;
        sf::Vector2f unitVector =
            diff / distance;  // TO DO: Make an implementation where we sum the
                              // vectors of two closest chaser entites and thus
                              // improve the escaping logic.
        sf::Vector2f dest = currentPos + unitVector * 15.f;
        if (isObstacleBetween(dest)) {
          dest = sf::Vector2f(dest.x + static_cast<float>(rand() % 20 - 10),
                              dest.y + static_cast<float>(rand() % 20 - 10));
        } else {
          destination = dest;
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
  game.addNutrient(getPos(), game.getNutrients().size());  // spawn a nutrient
  compost = true;
  game.removeEntity(id);  // remove entity from game.
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

unsigned int Entity::getId() { return id; }

float Entity::calcMovementCost() {
  return currentSpeed * getSize().x * getSize().y * agility /
         140.f;  // 140 is a arbitrary variable meant to keep the cost
                 // relatively low
}
