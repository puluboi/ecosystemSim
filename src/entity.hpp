#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <memory>

class Game;
class Entity {
 public: // CRAVING == chasing food.
  enum BehaviorState { IDLE, CHASING, FLEEING, CRAVING, DOWN_BAD  };

  BehaviorState currentBehavior = IDLE;

  std::shared_ptr<sf::RectangleShape> getShape();

  sf::Vector2f getPos();
  void setPos(sf::Vector2f pos);
  void setEnergy(int newEnergy);
  int getEnergy();
  sf::Vector2f getSize();
Entity(std::shared_ptr<sf::RectangleShape> shape_, Game& game_,
                      std::string id_, const Entity& parent1, const Entity& parent2);
void inheritVariables(const Entity& parent1, const Entity& parent2);
Entity(std::shared_ptr<sf::RectangleShape> shape_, Game& game_,
       std::string id_);  // if entity has no parents -- it's in the first wave.

bool collisionCheck();

bool isObstacleBetween(sf::Vector2f destination);

void initRandVariables();

void updateVariables();

void Update();

bool checkIfIdle();

void getRandomPos(int range);

void avoidCloseEntities();

bool isAlive();

void handleEntities();

bool chaseNearestEntity();

void getFurthestPos();

void chaseDecision(const sf::Vector2f& currentPos);

void fleeDecision(const sf::Vector2f& currentPos);

sf::Vector2f getDirection();

bool getDamaged(unsigned int damage_);

bool hasComposted();

void Compost();

void setChase(bool chase_);

void printState();

void calcNextPos(sf::Vector2f destination);
// Set's
void setAcceleration(float acceleration_);

unsigned int getThreat();
void setThreat(unsigned int input);

float getAggression();

void setSpeed(float speed_);
int getHealth();
float getEfficiency();
void setEfficiency(float efficiency_);
void setKillerEfficiency(float efficiency_);
void calcKeyValues();
void setWanderRange(int range_);

std::string getId();

bool toBeRemoved = false;  // mark for deferred removal

private:
// KEY VARIABLES:
unsigned int threatFactor;  // Entities with high threat factor are more likely
                            // to attack ones with lower -- comes from the
                            // health, damage, speed & agility
unsigned int senseFactor;   // How good senses does an entity possess -- good
                            // senses cost energy
float aggression;           // How aggressive an entity is. closer to 1 the more
                            // aggressive the entity is.

unsigned int energyUpkeep;  // Is calculated from all the positive attributes.
float calcMovementCost();
std::shared_ptr<sf::RectangleShape> shape;
sf::Vector2f destination = sf::Vector2f(0.f, 0.f);
sf::Vector2f nextpos;
sf::Vector2f lastpos;
float speed = 1;      // max speed
float agility = 0.3;  // agility: much speed is preserved when idle.
unsigned int maxEnergy = 20000;
unsigned int maxStamina = 10000;
unsigned int energy = 10000;
unsigned int stamina = 10000;
float efficiency =
    0.5;  // the efficiency of processing other entities for energy; if under
          // 0.5< the entity will seek to eat nutrients
float killerEfficiency = 1;  // the efficiency of the killer of this entity.
float currentSpeed = 0;      // Current speed of the entity
float acceleration = 0.01;   // The acceleration of the entity.
float minDistance = 140;     // Distance for spotting other entities
float damageDistance = 20;   // Distance required to deal damage
bool chase = false;          // If the entity chases other entities
int wanderRange = 300;       // How far the entity can get a random point
unsigned int damage = 10;    // How much damage the entity deals
int health = 100;            // how much health the entity has
int baseHealth = 100;
std::string id;        // entitys personal id
bool alive = true;     // Whether the entity is alive or...
bool compost = false;  // whether the entity has been composted
float dTime;           // time of death
Game& game;            // reference to the game
};