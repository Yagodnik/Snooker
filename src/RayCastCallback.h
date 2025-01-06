#pragma once

#include <Box2D/box2d.h>
#include <vector>

#ifdef DEBUG_DRAW
#include "../dummy/b2SFMLDraw.h"
#endif

class RayCastCallback final : public b2RayCastCallback {
public:
  RayCastCallback();

  float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;

  void Reset();
  void PassDebugDraw(void *drawer);

  bool m_hit;
  float m_angle;
  b2Vec2 m_point;
  b2Vec2 m_normal;
  b2Vec2 m_rayStart;
  b2Vec2 m_rayEnd;
  b2Body* m_ball;

  std::vector<b2Vec2> m_bodies_pos;

#ifdef DEBUG_DRAW
  b2SFMLDraw* m_draw;
#endif
};
