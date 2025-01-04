#pragma once

#include <Box2D/box2d.h>

class ContactListener final : public b2ContactListener {
  void BeginContact(b2Contact* contact) override;
  void EndContact(b2Contact* contact) override;
};
