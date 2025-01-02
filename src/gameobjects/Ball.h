#pragma once

#include <iostream>

#include "GameObject.h"
#include "../utils/PhysicsFactory.h"

constexpr float kVelocityThreshold = 0.1f;

enum class BallColor {
  kYellow = 0,
  kBlue = 1,
  kRed = 2,
  kPurple = 3,
  kOrange = 4,
  kGreen = 5,
  kBrown = 6,
  kBlack = 7
};

class Ball final : public GameObject {
public:
  explicit Ball(b2World& world, sf::Vector2f position);

  void SetColor(BallColor color);

  float GetX() const;
  float GetY() const;

  void MakeShot(float force, float angle);

  bool IsMoving() const;

  void Draw(sf::RenderWindow& window) override;
  void Update();

  bool CheckCollisionWith(b2Body* other) const;

private:
  sf::Sprite sprite_;
  std::shared_ptr<sf::Texture> texture_;
  b2Body* ball_body_;
};
