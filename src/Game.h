#pragma once

#include <SFML/Audio.hpp>
#include <vector>
#include <optional>
#include "gameobjects/Ball.h"
#include "gameobjects/ForceBar.h"
#include "gameobjects/PoolTable.h"
#include "gameobjects/BallsBar.h"

constexpr int kMaxHoldTime = 1500.f;

class Game final {
public:
  explicit Game(sf::RenderWindow& window);
  ~Game() = default;

  void HandleEvents(const std::optional<sf::Event>& event);
  void Update(float delta_time);
  void Render();

private:
  void PlaceBalls();
  void AddBallToCurrentPlayer(const Ball& ball);
  void SwitchTurn();

  sf::RenderWindow& window_;

  bool ball_in_hole_ = false;
  bool processing_shot_ = false;

  bool holding_{};
  sf::Time shot_start_;
  sf::Time hold_time_;
  float shot_force_{};

  sf::Clock clock_;

  b2World world_;

  int p1_scores_ = 0;
  int p2_scores_ = 0;

  BallsBar p1_bar_;
  BallsBar p2_bar_;
  ForceBar force_bar_;

  PoolTable pool_table_;
  Ball player_ball_;
  std::vector<Ball> balls_;

  PlayerNumber current_turn_;

  sf::Music music_;
  sf::SoundBuffer clapping_sound_buffer_;
  sf::Sound clapping_sound_;
};
