#pragma once

#include "GameObject.h"

class ForceBar final : public GameObject {
public:
  ForceBar();

  void SetPosition(float x, float y);
  void SetPercentage(float value);

  void Draw(sf::RenderWindow& window) override;

private:
  sf::Sprite power_sprite_;
  sf::Sprite bar_sprite_;
  std::shared_ptr<sf::Texture> texture_;

  sf::RectangleShape bar_background_;
  float value_;
  float total_width_;
};
