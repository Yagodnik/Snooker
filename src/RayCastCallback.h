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
  void SetAngle(float angle);
  void SetBallPosition(float x, float y);
  void AddBall(float x, float y);
  b2Vec2 GetHittedBallPosition() const;

  [[nodiscard]] bool HaveHit() const;
  [[nodiscard]] b2Vec2 GetHitPoint() const;
  [[nodiscard]] b2Vec2 GetHitNormal() const;

private:
#ifdef DEBUG_DRAW
  b2SFMLDraw* m_draw;
#endif

  std::vector<b2Vec2> ball_position_list_;
  b2Vec2 hitted_ball_position_;

  float angle_{};
  b2Vec2 ball_position_{};

  bool hit_;
  b2Vec2 point_;
  b2Vec2 normal_;
};
