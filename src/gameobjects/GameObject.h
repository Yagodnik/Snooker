#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

class GameObject {
public:
  virtual ~GameObject() = default;

  virtual void Draw(sf::RenderWindow& window) = 0;
  virtual void CollisionBegin(GameObject *other) {}
  virtual void CollisionEnd(GameObject *other) {}
};

