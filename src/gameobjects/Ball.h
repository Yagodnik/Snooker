#pragma once

#include <iostream>

#include <SFML/Audio.hpp>
#include "GameObject.h"
#include "../utils/PhysicsFactory.h"

constexpr float kVelocityThreshold = 0.1f;
constexpr int kBallRadius = 8;
constexpr int kColorsCount = 7;

enum class BallColor {
  Yellow = 0,
  Blue = 1,
  Red = 2,
  Purple = 3,
  Orange = 4,
  Green = 5,
  Brown = 6
};

sf::Color BallColorToColor(BallColor color);

class Ball final : public GameObject {
public:
  explicit Ball(b2World& world, sf::Vector2f position);

  void SetPosition(sf::Vector2f position);

  void SetColor(BallColor color);
  BallColor GetColor() const;

  float GetX() const;
  float GetY() const;

  void MakeShot(float force, float angle) const;

  bool IsMoving() const;

  void Draw(sf::RenderWindow& window) override;
  void Update();

  bool CheckCollisionWith(b2Body* other) const;

  void Disable() const;
  void PlayStrikeSound();

  void CollisionBegin(GameObject *other) override;

private:
  BallColor color_;
  sf::Sprite sprite_;
  std::shared_ptr<sf::Texture> texture_;
  b2Body* ball_body_;

  sf::SoundBuffer strike_sound_buffer_;
  sf::Sound strike_sound_;
};
