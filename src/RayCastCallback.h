#pragma once

#include <Box2D/box2d.h>

#include "../dummy/b2SFMLDraw.h"

class RayCastCallback final : public b2RayCastCallback {
public:
  RayCastCallback();

  float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;

  void Reset();

  bool m_hit;
  float m_angle;
  b2Vec2 m_point;
  b2Vec2 m_normal;
  b2Vec2 m_rayStart;
  b2Vec2 m_rayEnd;
  b2Body* m_ball;

  std::vector<b2Vec2> m_bodies_pos;

  b2SFMLDraw* m_draw;
};
