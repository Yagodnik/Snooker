#include "PoolTable.h"

#include "../utils/TextureManager.h"

PoolTable::PoolTable(b2World& world) :
  sprite_(kDummyTexture)
{
  const auto &texture_manager = TextureManager::GetInstance();
  texture_ = texture_manager.GetTexture("pool_table");

  sprite_.setTexture(*texture_, true);

  sprite_.setPosition({200, 200});
  sprite_.setScale(sf::Vector2f(2.f, 2.f));
  const float width = static_cast<float>(texture_->getSize().x) * sprite_.getScale().x;
  const float height = static_cast<float>(texture_->getSize().y) * sprite_.getScale().y;

  std::cout << width << " " << height << std::endl;

  const float left = 80.f;
  const float top = 32.f;

  sprite_.setPosition(sf::Vector2f(left, top));

  holes_[0] = PhysicsFactory::CreatHole(world, 20 + left, 20 + top, 5);
  holes_[1] = PhysicsFactory::CreatHole(world, width - 30 + left, 20 + top, 5);
  holes_[2] = PhysicsFactory::CreatHole(world, width - 30 + left, height - 30 + top, 5);
  holes_[3] = PhysicsFactory::CreatHole(world, 20 + left, height - 30 + top, 5);
  holes_[4] = PhysicsFactory::CreatHole(world, width / 2 - 5 + left, 12 + top, 5);
  holes_[5] = PhysicsFactory::CreatHole(world, width / 2 - 5 + left, height - 22 + top, 5);

  constexpr float radius = 0.2f;

  walls_[0] = PhysicsFactory::CreateRoundedWall(world, width / 4 + 10 + left, 20 + top, width / 2 - 60, 12, radius);
  walls_[1] = PhysicsFactory::CreateRoundedWall(world, 3 * width / 4 - 10 + left, 20 + top, width / 2 - 60, 12, radius);
  walls_[2] = PhysicsFactory::CreateRoundedWall(world, width / 4 + 10 + left, height - 16 + top, width / 2 - 60, 20, radius);
  walls_[3] = PhysicsFactory::CreateRoundedWall(world, 3 * width / 4 - 10 + left, height - 16 + top, width / 2 - 60, 20, radius);
  walls_[4] = PhysicsFactory::CreateRoundedWall(world, 14 + left, height / 2 + top, 20, height - 75, radius);
  walls_[5] = PhysicsFactory::CreateRoundedWall(world, width - 14 + left, height / 2 + top, 20, height - 75, radius);
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
