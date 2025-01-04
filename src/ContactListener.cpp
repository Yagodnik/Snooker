#include "ContactListener.h"

#include <iostream>
#include <ostream>

#include "Game.h"
#include "gameobjects/GameObject.h"

void ContactListener::BeginContact(b2Contact *contact) {
  std::cout << "ContactListener::BeginContact" << std::endl;

  b2Fixture* fixture_a = contact->GetFixtureA();
  b2Fixture* fixture_b = contact->GetFixtureB();

  b2Body* body_a = fixture_a->GetBody();
  b2Body* body_b = fixture_b->GetBody();

  GameObject* object_a = reinterpret_cast<GameObject*>(body_a->GetUserData().pointer);
  GameObject* object_b = reinterpret_cast<GameObject*>(body_b->GetUserData().pointer);

  std::cout << "object_a: " << object_a << std::endl;
  std::cout << "object_b: " << object_b << std::endl;

  if (object_a != nullptr) {
    object_a->CollisionBegin(object_b);
  }

  if (object_b != nullptr) {
    object_b->CollisionBegin(object_a);
  }
}

void ContactListener::EndContact(b2Contact *contact) {
  std::cout << "ContactListener::EndContact" << std::endl;

  b2Fixture* fixture_a = contact->GetFixtureA();
  b2Fixture* fixture_b = contact->GetFixtureB();

  b2Body* body_a = fixture_a->GetBody();
  b2Body* body_b = fixture_b->GetBody();

  // auto* object_a = reinterpret_cast<GameObject*>(body_a->GetUserData().pointer);
  // auto* object_b = reinterpret_cast<GameObject*>(body_b->GetUserData().pointer);
  //
  // if (object_a && object_b) {
  //   object_a->CollisionEnd(*object_b);
  //   object_b->CollisionEnd(*object_a);
  // }
}
