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
  Entity(std::shared_ptr<sf::RectangleShape> shape_, Game& game_,
         unsigned int id_);

  bool collisionCheck();

  bool isObstacleBetween(sf::Vector2f destination);

  void updateVariables();

  void Update();

  bool checkIfIdle();

  void getRandomPos(int range);

  void avoidCloseEntities();

  bool isAlive();

  void handleEntities();

  bool chaseNearestEntity();

  void getFurthestPos();

  bool getDamaged(unsigned int damage_);

  bool hasComposted();

  void Compost();

  void setChase(bool chase_);

  void calcNextPos(sf::Vector2f destination);
  //Set's
  void setAcceleration(float acceleration_);
  
  void setSpeed(float speed_);

  void setWanderRange(int range_);
  
  unsigned int getId();

 private:
 float calcMovementCost();
  std::shared_ptr<sf::RectangleShape> shape;
  sf::Vector2f destination = sf::Vector2f(0.f, 0.f);
  sf::Vector2f nextpos;
  sf::Vector2f lastpos;
  float speed = 1;      // max speed
  float agility = 0.7;  // agility: much speed is preserved when idle.
  unsigned int maxEnergy = 20000;
  unsigned int maxStamina = 10000;
  unsigned int energy = 10000;
  unsigned int stamina = 10000;
  float currentSpeed = 0;
  float acceleration = 0.01;
  float minDistance = 140;
  float aggroDistance = 200;
  float damageDistance = 20;
  bool chase = false;
  int wanderRange = 300;
  unsigned int damage = 100;
  int health = 100;
  unsigned int id;
  bool alive = true;
  bool compost = false;
  float dTime;  // time of death
  Game& game;
};