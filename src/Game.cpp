#include "Game.h"

Game::Game(sf::RenderWindow& window) :
  window_(window),
  world_({0.f, 0.f}),
  pool_table_(world_),
  player_ball_(world_, pool_table_.GetCenter()),
  p1_bar_(PlayerNumber::Player1),
  p2_bar_(PlayerNumber::Player2),
  current_turn_(PlayerNumber::Player1)
{
  force_bar_.SetPosition(80, 340);

  p1_bar_.SetPosition(32, 32);
  p2_bar_.SetPosition(592 , 32);

  PlaceBalls();
}

void Game::HandleEvents(const sf::Event &event) {
  if (event.is<sf::Event::MouseButtonReleased>() && !player_ball_.IsMoving()) {
    force_bar_.SetPercentage(0.0f);
    holding_ = false;

    const auto mouse_position = sf::Mouse::getPosition(window_);

    const float dx = static_cast<float>(mouse_position.x) - player_ball_.GetX();
    const float dy = static_cast<float>(mouse_position.y) - player_ball_.GetY();

    player_ball_.MakeShot(2 * shot_force_, std::atan2(dy, dx));
  }

  if (event.is<sf::Event::MouseButtonPressed>() && !player_ball_.IsMoving()) {
    holding_ = true;
    shot_start_ = clock_.getElapsedTime();
  }
}

void Game::Update(float delta_time) {
  if (holding_) {
    hold_time_ = clock_.getElapsedTime() - shot_start_;
    const int duration = (hold_time_.asMilliseconds() > kMaxHoldTime) ? kMaxHoldTime : hold_time_.asMilliseconds();
    shot_force_ = static_cast<float>(duration) / kMaxHoldTime;
  } else {
    shot_force_ = (shot_force_ > 0.0f) ? (shot_force_ - 0.01f) : 0.0f;
  }

  player_ball_.Update();
  force_bar_.SetPercentage(shot_force_);

  for (int i = 0;i < balls_.size();i++) {
    auto& ball = balls_[i];

    ball.Update();

    if (pool_table_.CheckHole(ball)) {
      balls_.erase(balls_.begin() + i);
      AddBallToCurrentPlayer();
    }
  }

  world_.Step(1 / 60.f, 4,4);
}

void Game::Render() {
  pool_table_.Draw(window_);
  pool_table_.DebugDraw(window_);

  p1_bar_.Draw(window_);
  p2_bar_.Draw(window_);
  force_bar_.Draw(window_);

  player_ball_.Draw(window_);
  for (auto& ball : balls_) {
    ball.Draw(window_);
  }
}

void Game::PlaceBalls() {
  const auto table_center = pool_table_.GetCenter();

  balls_.clear();

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < i + 1; j++) {
      balls_.emplace_back(world_, sf::Vector2f(i * 20 + table_center.x / 2, j * 20 + table_center.y - i * 10));
    }
  }
}

void Game::AddBallToCurrentPlayer() {
  if (current_turn_ == PlayerNumber::Player1) {
    p1_bar_.AddBall(BallColor::kGreen);
  } else {
    p2_bar_.AddBall(BallColor::kGreen);
  }
}
