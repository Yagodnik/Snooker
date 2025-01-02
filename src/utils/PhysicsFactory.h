#pragma once

#include <Box2D/box2d.h>

constexpr float kScale = 30.f;

class PhysicsFactory {
public:
  static b2Body* CreateBallBody(b2World& world_id, float x, float y, float r);
  static b2Body* CreatHole(b2World& world_id, float x, float y, float r);
  static b2Body* CreateRoundedWall(b2World& world_id, float x, float y, float w, float h, float r);
};
