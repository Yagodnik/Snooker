#include "RayCastCallback.h"

#include <algorithm>
#include <iostream>
#include <ostream>

#include "utils/PhysicsFactory.h"


RayCastCallback::RayCastCallback() :
  m_hit(false),
  m_normal(0, 0),
  m_point(0, 0)
{}

float RayCastCallback::ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float fraction) {
  if (fixture->IsSensor()) {
    return fraction;
  }

  b2World mini_world({0.f, 0.f});

#ifdef DEBUG_DRAW
  mini_world.SetDebugDraw(m_draw);
  m_draw->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit);
#endif

  for (int i = 0;i < m_bodies_pos.size();i++) {
    auto pos = m_bodies_pos[i];
    b2Body *temp = PhysicsFactory::CreateBallBody(mini_world, pos.x, pos.y, 8);
    temp->GetUserData().pointer = i + 1;
  }

  float px = m_ball->GetTransform().p.x * kScale;
  float py = m_ball->GetTransform().p.y * kScale;
  auto player_ball_copy = PhysicsFactory::CreateBallBody(mini_world, px, py, 8);
  player_ball_copy->GetUserData().pointer = 100;
  player_ball_copy->SetLinearDamping(0.f);

  float tx = point.x * kScale;
  float ty = point.y * kScale;

  const float dx = tx - px;
  const float dy = ty - py;
  // const float angle = std::atan2(dy, dx);
  const float angle = m_angle;
  const float imp_x = std::cos(angle);
  const float imp_y = std::sin(angle);

  float simulationTimeStep = 1.0f / 60.0f;
  int velocityIterations = 8;
  int positionIterations = 9;
  const int steps = 100;

  m_normal = b2Vec2(0, 0);
  m_point = point;

  player_ball_copy->ApplyLinearImpulseToCenter({imp_x, imp_y}, true);

  for (int i = 0; i < steps; ++i) {
    for (b2ContactEdge* contactEdge = player_ball_copy->GetContactList(); contactEdge; contactEdge = contactEdge->next) {
      b2Contact* contact = contactEdge->contact;

      if (!contact || !contact->IsTouching()) {
        continue;
      }

      b2Fixture* fixture_a = contact->GetFixtureA();
      b2Fixture* fixture_b = contact->GetFixtureB();

      b2Body* body_a = fixture_a->GetBody();
      b2Body* body_b = fixture_b->GetBody();

#ifdef DEBUG_DRAW
      m_draw->DrawCircle(body_a->GetPosition(), 8 / kScale, b2Color(0.8f, 0.8f, 0.8f));
      m_draw->DrawCircle(body_b->GetPosition(), 8 / kScale, b2Color(0.8f, 0.8f, 0.8f));
#endif

      if ((body_a->GetUserData().pointer != player_ball_copy->GetUserData().pointer && body_a->GetUserData().pointer != 0)) {
        m_hit = true;
        m_normal = body_a->GetLinearVelocity();
      }

      if ((body_b->GetUserData().pointer != player_ball_copy->GetUserData().pointer && body_b->GetUserData().pointer != 0)) {
        m_hit = true;
        m_normal = body_b->GetLinearVelocity();
      }

      m_normal.Normalize();

      if (m_hit) {
        b2WorldManifold worldManifold;
        contact->GetWorldManifold(&worldManifold);
        int pointCount = contact->GetManifold()->pointCount;
        if (pointCount > 0) {
#ifdef DEBUG_DRAW
          m_draw->DrawCircle(worldManifold.points[0], 8 / kScale, b2Color(0.8f, 0.8f, 0.8f));
#endif
          m_point = worldManifold.points[0];
        }

        return fraction;
      }
    }

    mini_world.Step(simulationTimeStep, velocityIterations, positionIterations);
  }

  return fraction;
}

void RayCastCallback::Reset() {
  m_hit = false;
  m_point = b2Vec2(0, 0);
  m_normal = b2Vec2(0, 0);
}

void RayCastCallback::PassDebugDraw(void *drawer) {
#ifdef DEBUG_DRAW
  m_draw = reinterpret_cast<b2SFMLDraw*>(drawer);
#endif

}
