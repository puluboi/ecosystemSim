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
  getRandomPos(wanderRange);
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
    getRandomPos(wanderRange);
  }

  handleEntities();
  if (!isObstacleBetween(destination)) {
    calcNextPos(destination);
    lastpos = shape->getPosition();
    if (collisionCheck()) {
        getRandomPos(wanderRange*0.2);
      std::cout << " collision" << std::endl;
    }
    shape->setPosition(nextpos);
  }
  else{
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
            getRandomPos(wanderRange);
          }
        }
      }

      if (!chase && distance < minDistance && entity->chase && entity->isAlive()) {
        tooClose = true;
        sf::Vector2f unitVector = diff / distance;
        sf::Vector2f dest = currentPos + unitVector * 30.f;
        if(isObstacleBetween(dest)){
            dest = sf::Vector2f(dest.x + static_cast<float>(rand() % 20 - 10), dest.y + static_cast<float>(rand() % 20 - 10));
        }
        else{
            destination = dest;
        }
      }
    }
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
