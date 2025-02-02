#include "game.hpp"

#include <string>

void Game::Update() {
  updateEntities();
  viz.Update();
  printAliveEntities();
  viz.Render();
}
bool Game::addObstacle(sf::Vector2f pos, sf::Vector2f size, sf::Color color) {
  auto shape = viz.addObject(pos, size, color);
  Obstacle obs = Obstacle(shape);
  obstacles.push_back(std::make_unique<Obstacle>(obs));
  return obs.getShape() != nullptr;
}

bool Game::addEntity(sf::Vector2f pos, sf::Vector2f size, sf::Color color) {
  auto shape = viz.addObject(pos, size, color);
  Entity ent = Entity(shape, *this,
                      entities.size() + 1);  // Pass the shared_ptr directly
  entities.push_back(std::make_unique<Entity>(ent));
  return ent.getShape() != nullptr;
}

bool Game::isRunning() { return viz.getWindowIsOpen(); }

bool Game::initGame() {
  initObstacles();
  initEntities();

  return true;
}
std::vector<std::unique_ptr<Obstacle>>& Game::getObstacles() {
  return obstacles;
}

void Game::initEntities() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0, 800);

  for (size_t i = 0; i < 100; i++) {
    float x = dis(gen);
    float y = dis(gen);
    addEntity(sf::Vector2f(x, y), sf::Vector2f(10.f, 10.f), sf::Color::Black);
    if (i % 5 == 0) {
      entities[i]->setChase(true);
      entities[i]->getShape()->setScale(sf::Vector2f(2.f,2.f));
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
  for (auto& entity : entities) {
    entity->Update();
  }
}
