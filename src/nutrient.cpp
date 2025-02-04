#include "nutrient.hpp"

Nutrient::Nutrient(std::shared_ptr<sf::RectangleShape> shape_, unsigned int id_)
    : shape(shape_), id(id_) {}