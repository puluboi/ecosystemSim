#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class Obstacle {
 public:
  Obstacle(std::shared_ptr<sf::RectangleShape> shape_);
  std::shared_ptr<sf::RectangleShape> getShape();

 private:
  sf::Vector2f pos;
  std::shared_ptr<sf::RectangleShape> shape;
};