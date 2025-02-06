#include "game.hpp"

#include <string>
#include <chrono>




void Game::Update() {
  updateEntities();
  viz.Update();
  printAliveEntities();
  viz.Render();
}
float Game::getDeltaTime(){
  return clock.getElapsedTime().asSeconds();
}
bool Game::addObstacle(sf::Vector2f pos, sf::Vector2f size, sf::Color color) {
  auto shape = viz.addObject(pos, size, color);
  Obstacle obs = Obstacle(shape);
  obstacles.push_back(std::make_unique<Obstacle>(obs));
  return obs.getShape() != nullptr;
}
void Game::removeEntity(unsigned int id){
  auto it = entities.begin();
  for (; it != entities.end(); it++) {
    if ((*it)->getId() == id) {
      break;
    }
  }

  if (it != entities.end()) {
    viz.removeObject((*it)->getShape());
    entities.erase(it);
  }
}

bool Game::addEntity(sf::Vector2f pos, sf::Vector2f size, sf::Color color) {
  auto shape = viz.addObject(pos, size, color);
  Entity ent = Entity(shape, *this,
                      entities.size() + 1);  // Pass the shared_ptr directly
  entities.push_back(std::make_unique<Entity>(ent));
  return ent.getShape() != nullptr;
}
bool Game::addNutrient(sf::Vector2f pos, unsigned int id){
  auto shape = viz.addObject(pos, sf::Vector2f(5.f,5.f), sf::Color(98,168,24));
  Nutrient nut = Nutrient(shape, id);
  nutrients.push_back(std::make_unique<Nutrient>(nut));
  return shape != nullptr;
}
void Game::removeNutrient(Nutrient nutrient){}

bool Game::isRunning() { return viz.getWindowIsOpen(); }

bool Game::initGame() {
  initObstacles();
  initEntities();

  return true;
}
std::vector<std::unique_ptr<Obstacle>>& Game::getObstacles() {
  return obstacles;
}
std::vector<std::unique_ptr<Nutrient>>& Game::getNutrients() {
  return nutrients;
}
void Game::initEntities() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0, 800);

  auto isInsideObstacle = [&](sf::Vector2f pos, sf::Vector2f size) {
    for (const auto& obstacle : obstacles) {
      auto obsPos = obstacle->getShape()->getPosition();
      auto obsSize = obstacle->getShape()->getSize();
      if (pos.x < obsPos.x + obsSize.x && pos.x + size.x > obsPos.x &&
          pos.y < obsPos.y + obsSize.y && pos.y + size.y > obsPos.y) {
        return true;
      }
    }
    return false;
  };

  for (size_t i = 0; i < 1000; i++) {
    float x, y;
    sf::Vector2f pos;
    do {
      x = dis(gen);
      y = dis(gen);
      pos = sf::Vector2f(x, y);
    } while (isInsideObstacle(pos, sf::Vector2f(10.f, 10.f)));

    addEntity(pos, sf::Vector2f(10.f, 10.f), sf::Color::Black);
    if (i % 20 == 0) {
      entities[i]->setChase(true);
      entities[i]->getShape()->setSize(sf::Vector2f(20.f, 20.f));
    }
  }
}

void Game::initObstacles() {
  addObstacle(sf::Vector2f(-100.f, -100.f), sf::Vector2f(1000.f, 50.f),
              sf::Color::Black);
  addObstacle(sf::Vector2f(-100.f, -100.f), sf::Vector2f(50.f, 1000.f),
              sf::Color::Black);
  addObstacle(sf::Vector2f(-100.f, 800.f), sf::Vector2f(1000.f, 50.f),
              sf::Color::Black);
  addObstacle(sf::Vector2f(800.f, -100.f), sf::Vector2f(50.f, 1000.f),
              sf::Color::Black);
  addObstacle(sf::Vector2f(300.f, 300.f), sf::Vector2f(200.f, 200.f),
              sf::Color::Black);
  addObstacle(sf::Vector2f(500.f, 400.f), sf::Vector2f(200.f, 200.f),
              sf::Color::Black);
  addObstacle(sf::Vector2f(20.f, 20.f), sf::Vector2f(100.f, 200.f),
              sf::Color::Black);
  addObstacle(sf::Vector2f(600.f, 20.f), sf::Vector2f(60.f, 60.f),
              sf::Color::Black);
  addObstacle(sf::Vector2f(500.f, 0.f), sf::Vector2f(50.f, 70.f),
              sf::Color::Black);
}

std::vector<std::unique_ptr<Entity>>& Game::getEntities() { return entities; }

Game::Game() { initGame(); }

void Game::printAliveEntities() {
  int aliveCount = 0;
  for (const auto& entity : entities) {
    if (entity->isAlive()) {
      aliveCount++;
    }
  }
  viz.alive = aliveCount;
}
void Game::updateEntities() {
  // First, update all entities (if still valid)
  for (auto& entity : entities) {
    if (entity != nullptr) {
      entity->Update();
    }
  }
  // Then, remove deleted (nullptr) entities
  entities.erase(std::remove(entities.begin(), entities.end(), nullptr), entities.end());
}
