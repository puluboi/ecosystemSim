#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <memory>

class Game;
class Entity {
 public:
  std::shared_ptr<sf::RectangleShape> getShape();

  sf::Vector2f getPos();
  void setPos(sf::Vector2f pos);
  sf::Vector2f getSize();
  Entity(std::shared_ptr<sf::RectangleShape> shape_, Game& game_, unsigned int id_);

  bool collisionCheck();

  bool isObstacleBetween(sf::Vector2f destination);

  void Update();

  bool checkIfIdle();

  void setWanderRange(int range_);

  void setSpeed(float speed_);

  void setAcceleration(float acceleration_);
  
  void getRandomPos(int range);

  void avoidCloseEntities();
  bool isAlive();

  void handleEntities();

  bool chaseNearestEntity();

  void getFurthestPos();

  bool getDamaged(unsigned int damage_);

  void setChase(bool chase_);

  void calcNextPos(sf::Vector2f destination);

  unsigned int getId();

 private:
  std::shared_ptr<sf::RectangleShape> shape;
  sf::Vector2f destination = sf::Vector2f(0.f,0.f);
  sf::Vector2f nextpos;
  sf::Vector2f lastpos;
  float speed = 1; //max speed
  float agility = 0.7; // agility: much speed is preserved when idle.
  float currentSpeed = 0;
  float acceleration = 0.01;
  float minDistance = 140;
  float aggroDistance = 200;
  float damageDistance = 40;
  bool chase = false;
  int wanderRange = 300;
  unsigned int damage = 100;
  int health = 100;
  unsigned int id;
  bool alive = true;
  Game& game;
};