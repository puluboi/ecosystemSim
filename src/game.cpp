#include "game.hpp"

#include <string>
#include <chrono>




void Game::Update() {
  int currentSecond = static_cast<int>(getDeltaTime());

  if(static_cast<int>(getDeltaTime()) % 2 == 1 && currentSecond != lastNutrientSpawnTime){
    for (size_t i = 0; i < 10; i++)
    {
      spawnNutrient();
    }
    
    lastNutrientSpawnTime = currentSecond;
    
  };
  
  // Handle entity clicking
  handleEntityClick();
  
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
void Game::removeEntity(std::string id){
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
void Game::spawnEntity(const std::unique_ptr<Entity>& parent1, const std::unique_ptr<Entity>& parent2){
  sf::Color p1c = parent1->getShape()->getFillColor();
  sf::Color p2c = parent1->getShape()->getFillColor();
  sf::Color color = sf::Color((p1c.a+p2c.r)/2, (p1c.b+p2c.g)/2, (p1c.b+p2c.b)/2, 255);
  
  auto shape = viz.addObject(parent1->getPos(),parent2->getSize() , color);
  Entity ent = Entity(shape, *this,
                      parent1->getId());  // Pass the shared_ptr directly

  ent.setEnergy(4000);
  entities.push_back(std::make_unique<Entity>(ent));
}
bool Game::addEntity(sf::Vector2f pos, sf::Vector2f size, sf::Color color, std::string id) {
  std::cout<<id<<std::endl;
  auto shape = viz.addObject(pos, size, color);
  Entity ent = Entity(shape, *this,
                      id);  // Pass the shared_ptr directly
  entities.push_back(std::make_unique<Entity>(ent));
  return ent.getShape() != nullptr;
}
bool Game::addNutrient(sf::Vector2f pos, unsigned int id, unsigned int energy){
  auto shape = viz.addObject(pos, sf::Vector2f(5.f,5.f), sf::Color(98,168,24));
  Nutrient nut = Nutrient(shape, id,energy);
  
  nutrients.push_back(std::make_unique<Nutrient>(nut));
  return shape != nullptr;
}
void Game::removeNutrient(unsigned int id){
  auto it = nutrients.begin();
  for (; it != nutrients.end(); it++) {
    if ((*it)->getId() == id) {
      break;
    }
  }

  if (it != nutrients.end()) {
    viz.removeObject((*it)->getShape());
    nutrients.erase(it);
  }
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

  for (size_t i = 0; i < 50; i++) {
    float x, y;
    sf::Vector2f pos;
    do {
      x = dis(gen);
      y = dis(gen);
      pos = sf::Vector2f(x, y);
    } while (isInsideObstacle(pos, sf::Vector2f(10.f, 10.f)));
    sf::Color randColor(
      static_cast<sf::Uint8>(static_cast<int>(dis(gen)) % 256),
      static_cast<sf::Uint8>(static_cast<int>(dis(gen)) % 256),
      static_cast<sf::Uint8>(static_cast<int>(dis(gen)) % 256)
    );
    std::string randomString;
    for (int i = 0; i < 4; ++i) {
      randomString += 'A' + static_cast<char>(static_cast<int>(dis(gen)) % 26);
    }
    addEntity(pos, sf::Vector2f(10.f, 10.f), randColor,randomString);
    //if (i % 99 == 0) {
    // entities[i]->getShape()->setSize(sf::Vector2f(20.f, 20.f));
    //}
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
      if(entity->getShape() == viz.getLastClickedObj()){
        std::cout << "Entity ID: " << entity->getId() << std::endl;
        std::cout << "Position: (" << entity->getPos().x << ", " << entity->getPos().y << ")" << std::endl;
        std::cout << "Size: (" << entity->getSize().x << ", " << entity->getSize().y << ")" << std::endl;
        std::cout << "Energy: " << entity->getEnergy() << std::endl;
        std::cout << "Alive: " << (entity->isAlive() ? "Yes" : "No") << std::endl;
      }
    }
  }
  // Remove entities marked for removal
  entities.erase(std::remove_if(entities.begin(), entities.end(), [&](const std::unique_ptr<Entity>& entity) {
    if (entity && entity->toBeRemoved) {
      viz.removeObject(entity->getShape());
      return true;
    }
    return false;
  }), entities.end());
  // Then, remove deleted (nullptr) entities
  entities.erase(std::remove(entities.begin(), entities.end(), nullptr), entities.end());
}
void Game::spawnNutrient() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0, 800);

  sf::Vector2f pos(dis(gen), dis(gen));
  
  unsigned int energy = static_cast<unsigned int>(dis(gen)) % 2500 + 500; // Random energy between 500 and 3000

  addNutrient(pos, getNutrients().size(), energy);
}

Entity* Game::findEntityAtPosition(sf::Vector2f position) {
  for (const auto& entity : entities) {
    if (entity && entity->isAlive()) {
      sf::FloatRect entityBounds = entity->getShape()->getGlobalBounds();
      if (entityBounds.contains(position)) {
        return entity.get();
      }
    }
  }
  return nullptr;
}

void Game::handleEntityClick() {
  if (viz.wasClicked()) {
    sf::Vector2f clickPos = viz.getClickPosition();
    Entity* clickedEntity = findEntityAtPosition(clickPos);
    
    if (clickedEntity) {
      selectedEntity = clickedEntity;
      std::string stats = getEntityStats(selectedEntity);
      viz.setEntityStats(stats);
    } else {
      // Clear selection if clicked on empty space
      selectedEntity = nullptr;
      viz.setEntityStats("");
    }
    
    viz.clearClick(); // Clear the click flag
  }
  
  // Update stats if we have a selected entity (in case it changes)
  if (selectedEntity && selectedEntity->isAlive()) {
    std::string stats = getEntityStats(selectedEntity);
    viz.setEntityStats(stats);
  } else if (selectedEntity && !selectedEntity->isAlive()) {
    // Clear selection if entity died
    selectedEntity = nullptr;
    viz.setEntityStats("");
  }
}

std::string Game::getEntityStats(const Entity* entity) {
  if (!entity) return "";
  
  std::string behaviorStr;
  switch (entity->currentBehavior) {
    case Entity::IDLE: behaviorStr = "IDLE"; break;
    case Entity::CHASING: behaviorStr = "CHASING"; break;
    case Entity::FLEEING: behaviorStr = "FLEEING"; break;
    case Entity::CRAVING: behaviorStr = "CRAVING"; break;
    case Entity::DOWN_BAD: behaviorStr = "DOWN_BAD"; break;
    default: behaviorStr = "UNKNOWN"; break;
  }
  
  return 
    "Selected Entity: " + entity->getId() + "\n" +
    "Behavior: " + behaviorStr + "\n" +
    "Health: " + std::to_string(entity->getHealth()) + "/" + std::to_string(entity->getBaseHealth()) + "\n" +
    "Energy: " + std::to_string(entity->getEnergy()) + "/" + std::to_string(entity->getMaxEnergy()) + "\n" +
    "Stamina: " + std::to_string(entity->getStamina()) + "/" + std::to_string(entity->getMaxStamina()) + "\n" +
    "Speed: " + std::to_string(static_cast<int>(entity->getSpeed() * 100)) + " (Current: " + std::to_string(static_cast<int>(entity->getCurrentSpeed() * 100)) + ")\n" +
    "Agility: " + std::to_string(static_cast<int>(entity->getAgility() * 100)) + "%\n" +
    "Efficiency: " + std::to_string(static_cast<int>(entity->getEfficiency() * 100)) + "%\n" +
    "Aggression: " + std::to_string(static_cast<int>(entity->getAggression() * 100)) + "%\n" +
    "Acceleration: " + std::to_string(static_cast<int>(entity->getAcceleration() * 1000)) + "\n" +
    "Threat: " + std::to_string(entity->getThreat()) + "\n" +
    "Damage: " + std::to_string(entity->getDamage()) + "\n" +
    "Position: (" + std::to_string(static_cast<int>(entity->getPos().x)) +
    ", " + std::to_string(static_cast<int>(entity->getPos().y)) + ")";
}