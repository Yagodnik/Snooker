#include "PoolTable.h"

#include "../utils/TextureManager.h"

PoolTable::PoolTable(b2World& world) :
  sprite_(kDummyTexture),
  left_(80.f),
  top_(32.f),
  scale_(2.f)
{
  const auto &texture_manager = TextureManager::GetInstance();
  texture_ = texture_manager.GetTexture("pool_table");

  sprite_.setTexture(*texture_, true);
  sprite_.setScale(sf::Vector2f(scale_, scale_));
  sprite_.setPosition(sf::Vector2f(left_, top_));

  for (int i = 0;i < kHoles.size(); i++) {
    auto& hole = kHoles[i];

    hole.x *= scale_;
    hole.y *= scale_;
    hole.x += left_;
    hole.y += top_;

    holes_[i] = PhysicsFactory::CreateHole(world, hole.x, hole.y, 5);
  }

  for (int i = 0;i < kWalls.size();i++) {
    auto& wall = kWalls[i];

    std::ranges::for_each(wall, [&](auto& vert) {
      vert.x /= kScale;
      vert.y /= kScale;
      vert.x *= scale_;
      vert.y *= scale_;
      vert.x += left_ / kScale;
      vert.y += top_ / kScale;
    });

    walls_[i] = PhysicsFactory::CreateWall(world, kWalls[i]);
  }
}

void PoolTable::Draw(sf::RenderWindow& window) {
  window.draw(sprite_);
}

void PoolTable::DebugDraw(sf::RenderWindow& window) const {
  sf::CircleShape hole_shape(5);
  hole_shape.setOrigin(sf::Vector2f(2.5f, 2.5f));
  hole_shape.setFillColor(sf::Color(0, 255, 0, 100));

  for (const auto hole : holes_) {
    const auto position = hole->GetPosition();
    hole_shape.setPosition({position.x * kScale, position.y * kScale});
    window.draw(hole_shape);
  }

  sf::RectangleShape wall_shape(sf::Vector2f(20, 20));
  wall_shape.setFillColor(sf::Color(0, 255, 0, 200));

  for (const auto wall : walls_) {
    const auto position = wall->GetPosition();

    const b2Fixture *fixture = wall->GetFixtureList();
    b2AABB aabb;
    if (fixture != nullptr) {
      const b2Shape *shape = fixture->GetShape();

      b2Transform transform = wall->GetTransform();
      shape->ComputeAABB(&aabb, transform, 0);
    }

    float width = std::abs(aabb.lowerBound.x - aabb.upperBound.x);
    float height = std::abs(aabb.lowerBound.y - aabb.upperBound.y);

    wall_shape.setSize({width * kScale, height * kScale});
    wall_shape.setOrigin(sf::Vector2f(wall_shape.getSize().x / 2, wall_shape.getSize().y / 2));
    wall_shape.setPosition({position.x * kScale, position.y * kScale});
    window.draw(wall_shape);
  }
}

bool PoolTable::CheckHole(const Ball &ball) const {
  return std::ranges::any_of(holes_, holes_ + kHolesCount, [&ball](b2Body *body) {
    return ball.CheckCollisionWith(body);
  });
}

sf::Vector2f PoolTable::GetCenter() const {
  const auto size = sprite_.getTexture().getSize();
  const auto scale = sprite_.getScale();
  const sf::Vector2f center = {size.x * scale.x / 2, size.y * scale.y / 2};

  return center + sprite_.getPosition();
}
