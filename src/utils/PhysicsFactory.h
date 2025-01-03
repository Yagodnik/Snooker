#pragma once

#include <Box2D/box2d.h>
#include <vector>

constexpr float kScale = 30.f;

class PhysicsFactory {
public:
  static b2Body* CreateBallBody(b2World& world_id, float x, float y, float r);
  static b2Body* CreateHole(b2World& world_id, float x, float y, float r);
  static b2Body* CreateRoundedWall(b2World& world_id, float x, float y, float w, float h, float r);
  static b2Body* CreateWall(b2World& world, const std::vector<b2Vec2>& vertices);
};
