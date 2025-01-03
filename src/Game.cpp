#include "Game.h"

Game::Game(sf::RenderWindow& window) :
  window_(window),
  world_({0.f, 0.f}),
  pool_table_(world_),
  player_ball_(world_, pool_table_.GetCenter()),
  p1_bar_(PlayerNumber::Player1),
  p2_bar_(PlayerNumber::Player2),
  current_turn_(PlayerNumber::Player1),
  clapping_sound_buffer_("assets/sounds/clapping.mp3"),
  clapping_sound_(clapping_sound_buffer_),
  music_("assets/sounds/snooker_music.mp3")
{
  music_.setLooping(true);
  music_.play();
  music_.setVolume(40);

  clapping_sound_.setVolume(50);

  force_bar_.SetPosition(80, 340);

  p1_bar_.SetPosition(32, 32);
  p2_bar_.SetPosition(592 , 32);

  PlaceBalls();
}

void Game::HandleEvents(const std::optional<sf::Event>& event) {
  if (!event.has_value()) {
    return;
  }

  if (const auto& mouse_event =
        event->getIf<sf::Event::MouseButtonReleased>()) {
    if (mouse_event->button == sf::Mouse::Button::Left) {
      force_bar_.SetPercentage(0.0f);
      holding_ = false;

      const auto mouse_position = sf::Mouse::getPosition(window_);

      const float dx = static_cast<float>(mouse_position.x) - player_ball_.GetX();
      const float dy = static_cast<float>(mouse_position.y) - player_ball_.GetY();

      player_ball_.MakeShot(2 * shot_force_, std::atan2(dy, dx));
      processing_shot_ = true;
      ball_in_hole_ = false;
    }
  }

  if (const auto& mouse_event =
        event->getIf<sf::Event::MouseButtonPressed>()) {
    if (mouse_event->button == sf::Mouse::Button::Left && !player_ball_.IsMoving()) {
      holding_ = true;
      shot_start_ = clock_.getElapsedTime();
    }

    if (mouse_event->button == sf::Mouse::Button::Right) {
      holding_ = false;
    }
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

  if (balls_.empty()) {
    if (p1_scores_ > p2_scores_) {
      std::cout << "P1 win" << std::endl;
    } else if (p2_scores_ > p1_scores_) {
      std::cout << "P2 win" << std::endl;
    } else {
      std::cout << "Draw" << std::endl;
    }
  }

  player_ball_.Update();
  force_bar_.SetPercentage(shot_force_);

  for (int i = 0;i < balls_.size();i++) {
    auto& ball = balls_[i];

    ball.Update();

    if (pool_table_.CheckHole(ball)) {
      clapping_sound_.play();
      clapping_sound_.setPlayingOffset(sf::seconds(19));

      AddBallToCurrentPlayer(ball);
      balls_.erase(balls_.begin() + i);
      ball_in_hole_ = true;
      processing_shot_ = false;
    }
  }

  bool processing_finished = std::ranges::all_of(balls_, [](auto& ball) {
    return !ball.IsMoving();
  });
  processing_finished = processing_finished && !player_ball_.IsMoving();

  if (processing_finished && !ball_in_hole_ && processing_shot_) {
    SwitchTurn();
    processing_shot_ = false;
  }

  world_.Step(1 / 60.f, 8,3);
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
      Ball ball(world_, sf::Vector2f(i * 20 + table_center.x / 2, j * 20 + table_center.y - i * 10));
      ball.SetColor(static_cast<BallColor>((i + j) % kColorsCount));
      balls_.push_back(ball);
    }
  }
}

void Game::AddBallToCurrentPlayer(const Ball& ball) {
  if (current_turn_ == PlayerNumber::Player1) {
    p1_bar_.AddBall(ball.GetColor());
    p1_scores_++;
  } else {
    p2_bar_.AddBall(ball.GetColor());
    p2_scores_++;
  }
}

void Game::SwitchTurn() {
  if (current_turn_ == PlayerNumber::Player1) {
    current_turn_ = PlayerNumber::Player2;
  } else {
    current_turn_ = PlayerNumber::Player1;
  }
}
