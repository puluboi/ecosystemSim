#pragma once

#include "memory.h"
#include "graphics/Viz.hpp"
#include <vector>
#include "obstacle.hpp"
#include "entity.hpp"
#include "nutrient.hpp"
#include <random>
#include <iostream>
class Game {
 public:
  void Update();
  bool addObstacle(sf::Vector2f pos, sf::Vector2f size, sf::Color color);
  void removeEntity(unsigned int id);
  bool addEntity(sf::Vector2f pos, sf::Vector2f size, sf::Color color);
  bool addNutrient(sf::Vector2f pos, unsigned int id);
  void removeNutrient(Nutrient nutrient);
  float getDeltaTime();
  bool isRunning();
  bool initGame();
  std::vector<std::unique_ptr<Obstacle>>& getObstacles();
  void initEntities();
  void initObstacles();
  std::vector<std::unique_ptr<Entity>>& getEntities();
  bool destroyEntity(const std::unique_ptr<Entity>& entity);
  Game();

  void printAliveEntities();

  void updateEntities();

 private:
 Viz viz;
sf::Clock clock;
  std::vector<std::unique_ptr<Entity>> entities;
  std::vector<std::unique_ptr<Obstacle>> obstacles;
  std::vector<std::unique_ptr<Nutrient>> nutrients;
};

