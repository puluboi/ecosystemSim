#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
class Game;
class Nutrient{
    public:
    unsigned int getEnergy();
    sf::Vector2f getPos();
    void setEnergy();
    Nutrient(std::shared_ptr<sf::RectangleShape> shape_, unsigned int id_,
             unsigned int energy_);
    std::shared_ptr<sf::RectangleShape> getShape();
    unsigned int getId();

   private:
    std::shared_ptr<sf::RectangleShape> shape;
    unsigned int energy;
    unsigned int id;
    float bornT;
};