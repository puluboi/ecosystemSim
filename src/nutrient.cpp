#include "nutrient.hpp"
#include "game.hpp"

Nutrient::Nutrient(std::shared_ptr<sf::RectangleShape> shape_, unsigned int id_, unsigned int energy_)
    : shape(shape_), id(id_), energy(energy_) {
    }
    std::shared_ptr<sf::RectangleShape> Nutrient::getShape() { return shape; }
    unsigned int Nutrient::getId() { return id; }
    unsigned int Nutrient::getEnergy(){ return energy; }
    sf::Vector2f Nutrient::getPos() { return shape->getPosition(); }