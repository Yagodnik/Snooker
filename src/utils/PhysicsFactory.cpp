#include "PhysicsFactory.h"

b2Body* PhysicsFactory::CreateBallBody(b2World& world, float x, float y, float r) {
  b2BodyDef body_def;
  body_def.type = b2_dynamicBody;
  body_def.position.Set(x / kScale, y / kScale);
  body_def.linearDamping = 0.3f;
  body_def.angularDamping = 0.3f;

  b2Body* body = world.CreateBody(&body_def);

  b2CircleShape circleShape;
  circleShape.m_radius = r / kScale;

  b2FixtureDef fixture_def;
  fixture_def.shape = &circleShape;
  fixture_def.restitution = 0.8f;
  fixture_def.density = 1.0f;
  fixture_def.friction = 1.0f;

  body->CreateFixture(&fixture_def);

  return body;
}

b2Body* PhysicsFactory::CreatHoleBody(b2World& world, float x, float y, float r) {
  b2BodyDef body_def;
  body_def.type = b2_kinematicBody;
  body_def.position.Set(x / kScale, y / kScale);

  b2Body* body = world.CreateBody(&body_def);

  b2CircleShape circleShape;
  circleShape.m_radius = r / kScale;

  b2FixtureDef fixture_def;
  fixture_def.shape = &circleShape;
  fixture_def.isSensor = true;

  body->CreateFixture(&fixture_def);

  return body;
}

b2Body* PhysicsFactory::CreateRoundedWall(b2World& world, float x, float y, float w, float h, float r) {
  b2BodyDef body_def;
  body_def.type = b2_staticBody;
  body_def.position.Set(x / kScale, y / kScale);

  b2Body* body = world.CreateBody(&body_def);

  b2PolygonShape polygonShape;
  polygonShape.SetAsBox(w / 2 / kScale, h / 2 / kScale, b2Vec2(0, 0), 0);

  b2FixtureDef fixture_def;
  fixture_def.shape = &polygonShape;
  fixture_def.restitution = 0.8f;

  body->CreateFixture(&fixture_def);

  return body;
}