#include "obstacle.hpp"


Obstacle::Obstacle(std::shared_ptr<sf::RectangleShape> shape_): shape(shape_) {

}

std::shared_ptr<sf::RectangleShape> Obstacle::getShape(){
    return shape;
}