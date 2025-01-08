#pragma once

#include "../utils/PhysicsFactory.h"
#include "Ball.h"
#include "GameObject.h"

constexpr int kHolesCount = 6;
constexpr int kWallsCount = 6;

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
  void InitFromJSON(b2World& world, const std::string& path);

  float left_;
  float top_;
  float scale_;

  sf::Sprite sprite_;

  b2Body* holes_[kHolesCount]{};
  b2Body* walls_[kWallsCount]{};
};

