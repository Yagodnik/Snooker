#pragma once

#include <SFML/Graphics.hpp>

class GameObject {
public:
  virtual ~GameObject() = default;

  virtual void Draw(sf::RenderWindow& window) = 0;
};

