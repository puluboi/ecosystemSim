#pragma once

#include <iostream>
#include <random>
#include <vector>

#include "entity.hpp"
#include "graphics/Viz.hpp"
#include "memory.h"
#include "nutrient.hpp"
#include "obstacle.hpp"
class Game {
 public:
  void Update();
  bool addObstacle(sf::Vector2f pos, sf::Vector2f size, sf::Color color);
  void removeEntity(std::string);
  void spawnEntity(const std::unique_ptr<Entity>& parent1,
                   const std::unique_ptr<Entity>& parent2);
  std::string idGen(std::string id1, std::string id2);
  bool addEntity(sf::Vector2f pos, sf::Vector2f size, sf::Color color,
                 std::string id);
  bool addNutrient(sf::Vector2f pos, unsigned int id, unsigned int energy);
  
  void removeNutrient(unsigned int id);
  float getDeltaTime();
  bool isRunning();
  bool initGame();
  std::vector<std::unique_ptr<Obstacle>>& getObstacles();
  std::vector<std::unique_ptr<Nutrient>>& getNutrients();
  void initEntities();
  void initObstacles();
  std::vector<std::unique_ptr<Entity>>& getEntities();
  bool destroyEntity(const std::unique_ptr<Entity>& entity);
  Game();

  void printAliveEntities();

  void updateEntities();

  void spawnNutrient();
  
  // Entity selection methods
  Entity* findEntityAtPosition(sf::Vector2f position);
  void handleEntityClick();
  std::string getEntityStats(const Entity* entity);

 private:
 int lastNutrientSpawnTime = -1; // Initialize to -1 to ensure first spawn works
  Entity* selectedEntity = nullptr; // Currently selected entity
  Viz viz;
  sf::Clock clock;
  std::vector<std::unique_ptr<Entity>> entities;
  std::vector<std::unique_ptr<Obstacle>> obstacles;
  std::vector<std::unique_ptr<Nutrient>> nutrients;
};
