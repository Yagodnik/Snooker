#include "Ball.h"

#include <iostream>
#include "../utils/TextureManager.h"

sf::Color BallColorToColor(BallColor color) {
  switch (color) {
  case BallColor::Yellow:
    return sf::Color::Yellow;
  case BallColor::Blue:
    return sf::Color::Blue;
  case BallColor::Red:
    return sf::Color::Red;
  case BallColor::Purple:
    return {170, 0, 170};
  case BallColor::Orange:
    return sf::Color::Cyan;
  case BallColor::Green:
    return sf::Color::Green;
  case BallColor::Brown:
    return sf::Color::Magenta;
  case BallColor::Black:
    return sf::Color::Black;
  default:
    return sf::Color::White;
  }
}

Ball::Ball(b2World& world, sf::Vector2f position) :
  sprite_(kDummyTexture)
{
  const auto &texture_manager = TextureManager::GetInstance();
  texture_ = texture_manager.GetTexture("balls");

  sprite_.setTexture(*texture_, true);
  sprite_.setTextureRect(
   sf::IntRect(sf::Vector2(16, 0), sf::Vector2(kBallRadius, kBallRadius))
  );
  sprite_.setPosition(position);
  sprite_.setScale({2, 2});
  sprite_.setOrigin(
    sf::Vector2f(static_cast<float>(kBallRadius) / 2, static_cast<float>(kBallRadius) / 2)
  );

  ball_body_ = PhysicsFactory::CreateBallBody(world, position.x, position.y, kBallRadius);
}

void Ball::SetColor(BallColor color) {
  color_ = color;

  sprite_.setColor(BallColorToColor(color));
}

BallColor Ball::GetColor() const {
  return color_;
}

float Ball::GetX() const {
  return sprite_.getPosition().x;
}

float Ball::GetY() const {
  return sprite_.getPosition().y;
}

void Ball::MakeShot(float force, float angle) {
  const float dx = force * std::cos(angle);
  const float dy = force * std::sin(angle);

  ball_body_->ApplyLinearImpulseToCenter({dx, dy}, true);
}

bool Ball::IsMoving() const {
  const auto velocity = ball_body_->GetLinearVelocity();
  const auto velocity_total = velocity.x * velocity.x + velocity.y * velocity.y;

  return (velocity_total >= kVelocityThreshold);
}

void Ball::Draw(sf::RenderWindow& window) {
  sf::Sprite shadow = sprite_;
  shadow.setColor(sf::Color(0, 0, 0, 80));
  shadow.move(sf::Vector2f(2.f, 2.f));

  window.draw(shadow);
  window.draw(sprite_);
}

void Ball::Update() {
  const auto position = ball_body_->GetPosition();

  if (!IsMoving()) {
    ball_body_->SetLinearVelocity({0.0f, 0.0f});
  }

  sprite_.setPosition({position.x * kScale, position.y * kScale});
}

bool Ball::CheckCollisionWith(b2Body* other) const {
  for (b2ContactEdge* contactEdge = ball_body_->GetContactList(); contactEdge; contactEdge = contactEdge->next) {
    b2Contact* contact = contactEdge->contact;

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();

    if (bodyA == other || bodyB == other) {
      return true;
    }
  }

  return false;
}