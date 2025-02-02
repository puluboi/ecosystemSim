#pragma once

#include "memory.h"
#include "graphics/Viz.hpp"
#include <vector>
#include "obstacle.hpp"
#include "entity.hpp"
#include <random>
class Game {
 public:
  void Update();
  bool addObstacle(sf::Vector2f pos, sf::Vector2f size, sf::Color color);
  bool addEntity(sf::Vector2f pos, sf::Vector2f size, sf::Color color);
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
  std::vector<std::unique_ptr<Entity>> entities;
  Viz viz;
  std::vector<std::unique_ptr<Obstacle>> obstacles;
};