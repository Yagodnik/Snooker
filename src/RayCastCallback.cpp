#include "RayCastCallback.h"

#include <algorithm>
#include <iostream>
#include <ostream>

#include "utils/PhysicsFactory.h"

RayCastCallback::RayCastCallback() :
  hit_(false),
  normal_(0, 0),
  point_(0, 0)
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

  for (int i = 0;i < ball_position_list_.size();i++) {
    auto pos = ball_position_list_[i];
    b2Body *temp = PhysicsFactory::CreateBallBody(mini_world, pos.x, pos.y, 8);
    temp->GetUserData().pointer = i + 1;
  }

  const float px = ball_position_.x;
  const float py = ball_position_.y;
  const auto player_ball_copy = PhysicsFactory::CreateBallBody(
    mini_world, px, py, 8
  );
  player_ball_copy->GetUserData().pointer = 100;
  player_ball_copy->SetLinearDamping(0.f);

  const float angle = angle_;
  const float imp_x = std::cos(angle);
  const float imp_y = std::sin(angle);

  const float timestep = 1.0f / 60.0f;
  const int velocity_iterations = 8;
  const int position_iterations = 9;
  const int computation_steps = 100;

  normal_ = b2Vec2(0, 0);
  point_ = point;

  player_ball_copy->ApplyLinearImpulseToCenter({imp_x, imp_y}, true);

  for (int i = 0; i < computation_steps; ++i) {
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
        hit_ = true;
        normal_ = body_a->GetLinearVelocity();
      }

      if ((body_b->GetUserData().pointer != player_ball_copy->GetUserData().pointer && body_b->GetUserData().pointer != 0)) {
        hit_ = true;
        normal_ = body_b->GetLinearVelocity();
      }

      normal_.Normalize();

      if (hit_) {
        b2WorldManifold world_manifold {};
        contact->GetWorldManifold(&world_manifold);

        int pointCount = contact->GetManifold()->pointCount;
        if (pointCount > 0) {
#ifdef DEBUG_DRAW
          m_draw->DrawCircle(worldManifold.points[0], 8 / kScale, b2Color(0.8f, 0.8f, 0.8f));
#endif
          point_ = world_manifold.points[0];
        }

        return fraction;
      }
    }

    mini_world.Step(timestep, velocity_iterations, position_iterations);
  }

  return fraction;
}

void RayCastCallback::Reset() {
  ball_position_list_.clear();
  hit_ = false;
  point_ = b2Vec2(0, 0);
  normal_ = b2Vec2(0, 0);
}

void RayCastCallback::PassDebugDraw(void *drawer) {
#ifdef DEBUG_DRAW
  m_draw = reinterpret_cast<b2SFMLDraw*>(drawer);
#endif
}

void RayCastCallback::SetAngle(float angle) {
  angle_ = angle;
}

void RayCastCallback::SetBallPosition(float x, float y) {
  ball_position_.x = x;
  ball_position_.y = y;
}

void RayCastCallback::AddBall(float x, float y) {
  ball_position_list_.emplace_back(x, y);
}

bool RayCastCallback::HaveHit() const {
  return hit_;
}

b2Vec2 RayCastCallback::GetHitPoint() const {
  return point_;
}

b2Vec2 RayCastCallback::GetHitNormal() const {
  return normal_;
}
