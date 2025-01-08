#include "BallsBar.h"

#include "../utils/ResourceManager.h"

BallsBar::BallsBar(PlayerNumber player) :
  position_(0, 0),
  player_id_(ResourceManager::GetTexture("gui")),
  ball_(ResourceManager::GetTexture("balls"))
{
  if (player == PlayerNumber::Player1) {
    player_id_.setTextureRect(sf::IntRect(
      sf::Vector2i(192, 10),
      sf::Vector2i(9, 5))
    );
  } else {
    player_id_.setTextureRect(sf::IntRect(
      sf::Vector2i(192, 18),
      sf::Vector2i(9, 5))
    );
  }

  player_id_.setScale(sf::Vector2f(2, 2));
}

void BallsBar::SetPosition(float x, float y) {
  position_.x = x;
  position_.y = y;
}

void BallsBar::Draw(sf::RenderWindow &window) {
  player_id_.setPosition(position_);
  window.draw(player_id_);

  ball_.setScale(sf::Vector2f(2, 2));
  ball_.setTextureRect(sf::IntRect(sf::Vector2(8, 0), sf::Vector2i(8, 8)));

  for (int i = 0;i < ball_colors_.size();i++) {
    const auto ball_color = ball_colors_[i];

    ball_.setColor(BallColorToColor(ball_color));
    ball_.setPosition(sf::Vector2f(position_.x, position_.y + 20 + static_cast<float>(i) * 18));
    window.draw(ball_);
  }
}

void BallsBar::AddBall(const BallColor color) {
  ball_colors_.push_back(color);
}