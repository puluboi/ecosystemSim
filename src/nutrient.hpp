#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class Nutrient{
    public:
    Nutrient(std::shared_ptr<sf::RectangleShape> shape_, unsigned int id_);
    private:
    std::shared_ptr<sf::RectangleShape> shape;
    unsigned int id;
};