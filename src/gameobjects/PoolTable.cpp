#include "PoolTable.h"

#include <fstream>
#include "../utils/TextureManager.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

PoolTable::PoolTable(b2World& world) :
  sprite_(kDummyTexture),
  left_(0),
  top_(0),
  scale_(0)
{
  InitFromJSON(world, "configs/pool_table.json");

  const auto &texture_manager = TextureManager::GetInstance();
  texture_ = texture_manager.GetTexture("pool_table");

  sprite_.setTexture(*texture_, true);
  sprite_.setScale(sf::Vector2f(scale_, scale_));
  sprite_.setPosition(sf::Vector2f(left_, top_));
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

void PoolTable::InitFromJSON(b2World& world, const std::string &path) {
  std::ifstream config_file(path);
  if (!config_file.is_open()) {
    std::cerr << "Failed to open file " << path << std::endl;
    return;
  }

  json data = json::parse(config_file);

  config_file.close();

  if (!data.contains("position") || !data.contains("scale")) {
    std::cerr << "Failed to parse file " << path << std::endl;
    return;
  }

  left_ = data["position"]["left"].get<float>();
  top_ = data["position"]["top"].get<float>();
  scale_ = data["scale"].get<float>();

  if (!data.contains("holes") || !data.contains("walls")) {
    std::cerr << "Failed to parse file " << path << std::endl;
    return;
  }

  for (int i = 0;i < data["holes"].size();i++) {
    const auto &item = data["holes"][i];

    auto hole_x = static_cast<float>(item["x"].get<int>());
    auto hole_y = static_cast<float>(item["y"].get<int>());

    hole_x *= scale_;
    hole_y *= scale_;
    hole_x += left_;
    hole_y += top_;

    holes_[i] = PhysicsFactory::CreateHole(world, hole_x, hole_y, 5);
  }

  for (int i = 0;i < data["walls"].size();i++) {
    auto& wall = data["walls"][i];

    std::vector<b2Vec2> wall_vertices;

    for (const auto &item : wall) {
      wall_vertices.emplace_back(item["x"].get<int>(), item["y"].get<int>());
    }

    std::ranges::for_each(wall_vertices, [&](auto& vert) {
      vert.x /= kScale;
      vert.y /= kScale;
      vert.x *= scale_;
      vert.y *= scale_;
      vert.x += left_ / kScale;
      vert.y += top_ / kScale;
    });

    walls_[i] = PhysicsFactory::CreateWall(world, wall_vertices);
  }
}
