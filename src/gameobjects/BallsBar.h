#pragma once

#include "GameObject.h"
#include "Ball.h"

enum class PlayerNumber {
  Player1,
  Player2
};

class BallsBar final : public GameObject {
public:
  BallsBar(PlayerNumber player = PlayerNumber::Player1);

  void SetPosition(float x, float y);
  void Draw(sf::RenderWindow &window) override;

  void AddBall(BallColor color);

private:
  sf::Vector2f position_;

  sf::Sprite ball_;
  sf::Sprite player_id_;

  std::vector<BallColor> ball_colors_;
};
