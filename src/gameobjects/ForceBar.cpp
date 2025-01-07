#include "ForceBar.h"

#include "../utils/TextureManager.h"

ForceBar::ForceBar() :
  power_sprite_(kDummyTexture),
  bar_sprite_(kDummyTexture),
  value_(0.f)
{
  const auto &texture_manager = TextureManager::GetInstance();
  texture_ = texture_manager.GetTexture("gui");

  if (!texture_) {
    std::cerr << "Cant get texture from gui" << std::endl;
    return;
  }

  bar_sprite_.setTexture(*texture_);
  bar_sprite_.setTextureRect(sf::IntRect(sf::Vector2(184, 0), sf::Vector2(8, 24)));
  bar_sprite_.setScale(sf::Vector2f(2, 2));
  bar_sprite_.setRotation(sf::degrees(-90.0f));
  bar_sprite_.setPosition(sf::Vector2f(300, 300));

  power_sprite_.setTexture(*texture_);
  power_sprite_.setTextureRect(sf::IntRect(sf::Vector2(192, 2), sf::Vector2(15, 5)));
  power_sprite_.setScale(sf::Vector2f(2, 2));

  total_width_ = 48;

  bar_background_.setFillColor(sf::Color::Red);
  bar_background_.setPosition(sf::Vector2f(300, 300 - 14));
  bar_background_.setSize(sf::Vector2f(0, 14));
}

void ForceBar::SetPosition(float x, float y) {
  power_sprite_.setPosition({x, y - 12});
  bar_sprite_.setPosition({x + 36, y});
  bar_background_.setPosition(sf::Vector2f(x + 36, y - 14));
}

void ForceBar::SetPercentage(float value) {
  if (value < 0.0f || value > 1.0f) {
    return;
  }

  value_ = value;
}

void ForceBar::Draw(sf::RenderWindow& window) {
  window.draw(power_sprite_);

  const auto original_size = bar_background_.getSize();
  bar_background_.setSize(sf::Vector2f(total_width_ * value_, original_size.y));
  window.draw(bar_background_);
  window.draw(bar_sprite_);
}
