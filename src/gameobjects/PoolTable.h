#pragma once

#include "../utils/PhysicsFactory.h"
#include "Ball.h"
#include "GameObject.h"

constexpr int kHolesCount = 6;
constexpr int kWallsCount = 6;

static std::vector<b2Vec2> kHoles = {
{12, 12},
{120, 9},
{228, 12},
{228, 124},
{120, 127},
{12, 124}
};

static std::vector<std::vector<b2Vec2>> kWalls = {
  {
    {16, 8},
    {115, 8},
    {113, 13},
    {20, 13}
  },
  {
    {8, 16},
    {13, 20},
    {13, 116},
    {8, 120}
  },
{
  {16, 128},
  {20, 123},
  {113, 123},
  {114, 128}
  },
{
  {126, 128},
  {127, 123},
  {220, 123},
  {224, 128}
  },
{
  {232, 120},
  {227, 116},
  {227, 20},
  {232, 16}
  },
{
  {220, 13},
  {224, 8},
  {126, 8},
  {127, 13}
  }
};

class PoolTable final : public GameObject {
public:
  explicit PoolTable(b2World& world);
  PoolTable(const PoolTable&) = delete;
  PoolTable& operator=(const PoolTable&) = delete;

  void Draw(sf::RenderWindow &window) override;
  void DebugDraw(sf::RenderWindow& window) const;

  bool CheckHole(const Ball& ball) const;

  sf::Vector2f GetCenter() const;

private:
  float left_;
  float top_;
  float scale_;

  sf::Sprite sprite_;
  std::shared_ptr<sf::Texture> texture_;

  b2Body* holes_[kHolesCount]{};
  b2Body* walls_[kWallsCount]{};
};

