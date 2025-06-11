#include "Viz.hpp"

#include <iostream>
#include <memory>

// private functions
void Viz::initVariables() { 
  this->window = nullptr; }

void Viz::initObjects() {
  // Example of adding objects to the game
  // add "crosshair" object
  sf::Vector2f pos1(0.f, 0.f);
  sf::Vector2f size1(10.f, 10.f);
  sf::Color color1 = sf::Color(0, 0, 0, 100);
  this->addObject(pos1, size1, color1);

}

void Viz::initWindow(int width, int height, bool fullscreen) {
  this->videoMode.height = height;
  this->videoMode.width = width;

  this->backgroundColor = sf::Color(255, 215, 0, 255);
  if (fullscreen) {
    this->window = new sf::RenderWindow(this->videoMode, "Visualisation",
                                        sf::Style::Fullscreen);
  } else {
    this->window = new sf::RenderWindow(this->videoMode, "Visualisation");
  }
  this->view = window->getView();
  this->viewSpeed = 100;
  window->setFramerateLimit(60);
}

void Viz::updateViewPos() {
  sf::Vector2i mousePosition = sf::Mouse::getPosition(*window);
  if (mousePosition.x < window->getSize().x * 0.1) {
    view.move(-viewSpeed * deltaTime.asSeconds(), 0);
  } else if (mousePosition.x >
             window->getSize().x - window->getSize().x * 0.1) {
    view.move(viewSpeed * deltaTime.asSeconds(), 0);
  }

  if (mousePosition.y < window->getSize().y * 0.1) {
    view.move(0, -viewSpeed * deltaTime.asSeconds());
  } else if (mousePosition.y >
             window->getSize().y - window->getSize().y * 0.1) {
    view.move(0, viewSpeed * deltaTime.asSeconds());
  }
  changeObjectPosition(objects[0].get(), view.getCenter());
}

void Viz::changeObjectPosition(sf::RectangleShape* object,
                               const sf::Vector2f& newPosition) {
  if (object) {
    object->setPosition(newPosition);
    // std::cout << "Object position changed to (" << newPosition.x << ", "
    //           << newPosition.y << ")" << std::endl;
  } else {
    // std::cerr << "Error: Null object pointer" << std::endl;
  }
}

// Accessors
const bool Viz::getWindowIsOpen() const { return this->window->isOpen(); }

// Public Functions
Viz::Viz() {
  this->initVariables();
  this->initWindow(1400, 800, false);
  this->initObjects();
}
Viz::~Viz() { delete this->window; }

void Viz::pollEvents() {
  while (this->window->pollEvent(this->ev)) {
    switch (this->ev.type) {
      case sf::Event::Closed:
        this->window->close();
        break;
      case sf::Event::KeyPressed:
        if (this->ev.key.code == sf::Keyboard::Escape) this->window->close();
        break;
      case sf::Event::MouseButtonPressed:
        if (this->ev.mouseButton.button == sf::Mouse::Left) {
          // Convert screen coordinates to world coordinates
          sf::Vector2i mousePos(this->ev.mouseButton.x, this->ev.mouseButton.y);
          lastClickPos = this->window->mapPixelToCoords(mousePos);
          hasClick = true;
        }
        break;
    }
  }
}

void Viz::Update() {
  this->deltaTime = clock.restart();  // update the delta time
  this->updateViewPos();              // update the position of the camera
  this->handleMouseClick();
  this->pollEvents();                 // check all the events.
  
}
/*
    Renders the objects and clears the old frame.
*/
void Viz::Render() {
  this->window->setView(view);
  this->window->clear(backgroundColor);

  // Draw visualisation objects
  for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
    this->window->draw(**it);
  }
  std::string coords = std::to_string(static_cast<int>(view.getCenter().x)) + ", " + std::to_string(static_cast<int>(view.getCenter().y))+ '\n' + std::to_string(alive);
  printToScreen(coords, sf::Vector2f(10.f,10.f));
  
  // Display entity stats if available
  displayEntityStats();
  
  this->window->display();
}
/*
Adds an object to the render que;
*/
std::shared_ptr<sf::RectangleShape> Viz::addObject(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Color& color) {
    auto newObj = std::make_shared<sf::RectangleShape>(size);
    newObj->setFillColor(color);
    newObj->setPosition(pos);

    objects.push_back(newObj);

    return newObj;
}
/*
Remove an object from the render que;
*/
bool Viz::removeObject(std::shared_ptr<sf::RectangleShape> obj) {
  auto it = std::find_if(objects.begin(), objects.end(),
                         [obj](const std::shared_ptr<sf::RectangleShape>& o) {  // Changed to std::shared_ptr
                           return o == obj;
                         });
  if (it != objects.end()) {
    objects.erase(it);
    return true;
  }
  return false;
}

void Viz::printToScreen(std::string text, sf::Vector2f pos) {
  sf::Font font;
  if (!font.loadFromFile("Roboto/Roboto-Bold.ttf")) {
    std::cerr << "Error loading font" << std::endl;
    return;
  }

  sf::Text sfText;
  sfText.setFont(font);
  sfText.setString(text);
  sfText.setCharacterSize(24);
  sfText.setFillColor(sf::Color::Black);
  sf::FloatRect textRect = sfText.getLocalBounds();
  sfText.setOrigin(textRect.width, 0);

  sf::Vector2f viewTopRight =
      window->mapPixelToCoords(sf::Vector2i(window->getSize().x, 0));
  sfText.setPosition(viewTopRight.x - pos.x, viewTopRight.y + pos.y);

  window->draw(sfText);
}
std::shared_ptr<sf::RectangleShape> Viz::getLastClickedObj(){
  return lastClickedObject;
}
void Viz::handleMouseClick() {
  if (hasClick) {
    for (auto& obj : objects) {
      if (obj->getGlobalBounds().contains(lastClickPos)) {
        std::cout << "Object clicked at position: (" << lastClickPos.x << ", " << lastClickPos.y << ")" << std::endl;
        lastClickedObject = obj;
        break;
      }
    }
  }
}

sf::Vector2f Viz::getClickPosition() {
  return lastClickPos;
}

bool Viz::wasClicked() {
  return hasClick;
}

void Viz::clearClick() {
  hasClick = false;
}

void Viz::setEntityStats(const std::string& stats) {
  entityStatsText = stats;
}

void Viz::displayEntityStats() {
  if (!entityStatsText.empty()) {
    sf::Font font;
    if (!font.loadFromFile("Roboto/Roboto-Bold.ttf")) {
      std::cerr << "Error loading font" << std::endl;
      return;
    }

    sf::Text sfText;
    sfText.setFont(font);
    sfText.setString(entityStatsText);
    sfText.setCharacterSize(18);
    sfText.setFillColor(sf::Color::White);
    
    // Position the stats panel on the left side
    sf::Vector2f viewTopLeft = window->mapPixelToCoords(sf::Vector2i(0, 0));
    sfText.setPosition(viewTopLeft.x + 10, viewTopLeft.y + 60);

    // Draw a background rectangle for better readability
    sf::FloatRect textBounds = sfText.getLocalBounds();
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(textBounds.width + 20, textBounds.height + 20));
    background.setPosition(viewTopLeft.x + 5, viewTopLeft.y + 55);
    background.setFillColor(sf::Color(0, 0, 0, 180)); // Semi-transparent black

    window->draw(background);
    window->draw(sfText);
  }
}