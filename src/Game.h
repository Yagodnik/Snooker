#pragma once

#include <vector>
#include "gameobjects/Ball.h"
#include "gameobjects/ForceBar.h"
#include "gameobjects/PoolTable.h"
#include "gameobjects/BallsBar.h"

constexpr int kMaxHoldTime = 1500.f;

class Game final {
public:
  explicit Game(sf::RenderWindow& window);
  ~Game() = default;

  void HandleEvents(const sf::Event& event);
  void Update(float delta_time);
  void Render();

private:
  void PlaceBalls();
  void AddBallToCurrentPlayer();

  sf::RenderWindow& window_;

  bool holding_{};
  sf::Time shot_start_;
  sf::Time hold_time_;
  float shot_force_{};

  sf::Clock clock_;

  b2World world_;

  BallsBar p1_bar_;
  BallsBar p2_bar_;
  ForceBar force_bar_;

  PoolTable pool_table_;
  Ball player_ball_;
  std::vector<Ball> balls_;

  PlayerNumber current_turn_;
};