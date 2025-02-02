#ifndef CPP_PROJECT_VIZ
#define CPP_PROJECT_VIZ

#include <memory>
#include <stdio.h>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class Viz {
  /* Class for visualization of the simulator
   */
public:
int alive;
 private:
  // private variables
  // Window
  sf::RenderWindow* window;
  sf::VideoMode videoMode;
  sf::Event ev;
  sf::View view;
  sf::Clock clock;
  // time between frames
  sf::Time deltaTime;

  // Private functions
  void initVariables();
  void initObjects();
  void initWindow(int width, int height, bool fullscreen);
  void updateViewPos();

  // Game objects
  sf::RectangleShape obj;
  std::vector<std::shared_ptr<sf::RectangleShape>> objects;

 public:
  // Variables
  int viewSpeed;  // Speed at which the camera moves

  sf::Color backgroundColor;
  // Accessors
  const bool getWindowIsOpen() const;

  // Functions
  Viz();
  virtual ~Viz();
  // Checks for all the IO inputs
  void pollEvents();
  // Updates the visualisation
  void Update();
  // Renders the updated visualisation
  void Render();

  /*
      size and color of the object as an input and adds the object to the
     objects vector
      */
  std::shared_ptr<sf::RectangleShape> addObject(const sf::Vector2f& pos,
                                                const sf::Vector2f& size,
                                                const sf::Color& color);
  // Changes the position of and object
  void changeObjectPosition(sf::RectangleShape* object,
                            const sf::Vector2f& newPosition);
  bool removeObject(std::shared_ptr<sf::RectangleShape> obj);
  /*
    Remove an object from the render que;
    */
  void printToScreen(std::string text, sf::Vector2f pos);
};
#endif
