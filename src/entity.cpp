#include "entity.hpp"

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
  getRandomPos();
}

bool Entity::collisionCheck() {
  for (const auto& entity : game.getEntities()) {
    if (entity->getId() != id && entity->isAlive()) {
      sf::FloatRect entityBounds = entity->getShape()->getGlobalBounds();
      nextpos;
      if (entityBounds.contains(nextpos)) {
        nextpos = getPos();
        entity->getDamaged(0.5 * damage);
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

void Entity::Update() {
  if (!alive) {
    return;
  }
  if (checkIfIdle()) {
    currentSpeed *= agility;
    getRandomPos();
  }

  handleEntities();
  if (!isObstacleBetween(destination)) {
    calcNextPos(destination);
    lastpos = shape->getPosition();
    if (collisionCheck()) {
      std::cout << " collision" << std::endl;
    }
    shape->setPosition(nextpos);
  }
  else{
    getRandomPos();
  }
}

bool Entity::checkIfIdle() {
  sf::Vector2f currentPos = shape->getPosition();
  return abs(destination.x - currentPos.x) < 0.5 &&
         abs(destination.y - currentPos.y) < 0.5;
}

void Entity::setWanderRange(int range_) { wanderRange = range_; }

void Entity::setSpeed(float speed_) { speed = speed_; }

void Entity::setAcceleration(float acceleration_) {
  acceleration = acceleration_;
}

void Entity::getRandomPos() {
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
  if (!isObstacleBetween(dest)) {
    destination = dest;
  } else {
    destination = lastpos;
  }
}

bool Entity::isAlive() { return alive; }

void Entity::handleEntities() {
  sf::Vector2f currentPos = shape->getPosition();
  sf::Vector2f moveAway(0.0f, 0.0f);
  bool tooClose = false;
  float nearestDistance = aggroDistance;

  for (const auto& entity : game.getEntities()) {
    if (entity->getId() != id) {
      sf::Vector2f otherPos = entity->getPos();
      sf::Vector2f diff = currentPos - otherPos;
      float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

      if (chase && entity->isAlive() && distance < nearestDistance &&
          distance < aggroDistance && !isObstacleBetween(otherPos)) {
        nearestDistance = distance;
        destination = otherPos;
        if (distance < damageDistance) {
          if (entity->getDamaged(damage)) {  // if the target dies
            getRandomPos();
          }
        }
      }

      if (!chase && distance < minDistance && entity->isAlive()) {
        tooClose = true;
        moveAway += diff / distance;  // Normalize and add to moveAway vector
      }
    }
  }

  if (!chase && tooClose) {
    moveAway /=
        static_cast<float>(game.getEntities().size() - 1);  // Average direction
    destination = currentPos + moveAway * minDistance;
  }
}

bool Entity::getDamaged(unsigned int damage_) {
  health -= damage_;
  if (health <= 0) {
    setAcceleration(0);
    shape->setFillColor(sf::Color::Red);
    alive = false;
    return true;
  }
  return false;
}

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
