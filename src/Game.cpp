#include "Game.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "utils/ResourceManager.h"

using json = nlohmann::json;

Game::Game(sf::RenderWindow& window) :
  window_(window),
  world_({0.f, 0.f}),
  p1_bar_(PlayerNumber::Player1),
  p2_bar_(PlayerNumber::Player2),
  pool_table_(world_),
  player_ball_(world_, pool_table_.GetCenter()),
  current_turn_(PlayerNumber::Player1),
  music_("assets/sounds/snooker_music.mp3"),
  clapping_sound_(ResourceManager::GetSound("clapping")),
  sad_sound_(ResourceManager::GetSound("sad")),
  end_sound_(ResourceManager::GetSound("end")),
  game_finished_(false),
  is_draw_(false),
  p1_win_(false),
  pixel_font_("assets/fonts/Jersey25-Regular.ttf"),
  win_text_(pixel_font_),
  restart_text_(pixel_font_),
#ifdef DEBUG_DRAW
  debug_draw_(window_, 30.f),
#endif
  holding_(false),
  shot_angle_(0.f),
  shot_force_(0.f),
  force_coefficient_(1.f),
  timestep_(1 / 60.f),
  velocity_iterations_(8),
  position_iterations_(3)
{
  world_.SetContactListener(&contact_listener_);
#ifdef DEBUG_DRAW
  world_.SetDebugDraw(&debug_draw_);
  debug_draw_.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit);
#endif

  music_.setLooping(true);
  music_.play();

  ResetGame();

  blink_start_time_ = clock_.getElapsedTime();
}

void Game::HandleEvents(const std::optional<sf::Event>& event) {
  if (!event.has_value()) {
    return;
  }

  const auto mouse_position = sf::Mouse::getPosition(window_);

  const float dx = static_cast<float>(mouse_position.x) - player_ball_.GetX();
  const float dy = static_cast<float>(mouse_position.y) - player_ball_.GetY();

  shot_angle_ = std::atan2(dy, dx);

  if (const auto& mouse_event =
        event->getIf<sf::Event::MouseButtonReleased>()) {
    if (mouse_event->button == sf::Mouse::Button::Left) {
      force_bar_.SetPercentage(0.0f);
      holding_ = false;

      player_ball_.MakeShot(force_coefficient_ * shot_force_, std::atan2(dy, dx));
      processing_shot_ = true;
      ball_in_hole_ = false;
    }
  }

  if (const auto& mouse_event =
        event->getIf<sf::Event::MouseButtonPressed>()) {
    if (mouse_event->button == sf::Mouse::Button::Left && IsMovementFinished()) {
      holding_ = true;
      shot_start_ = clock_.getElapsedTime();
    }

    if (mouse_event->button == sf::Mouse::Button::Right) {
      holding_ = false;
    }
  }

  if (const auto& keyboard_event =
        event->getIf<sf::Event::KeyPressed>()) {
    if (keyboard_event->code == sf::Keyboard::Key::Space && game_finished_) {
      ResetGame();
    }
  }
}

void Game::Update(float delta_time) {
  if (clock_.getElapsedTime() - blink_start_time_ > sf::seconds(0.6f)) {
    blink_start_time_ = clock_.getElapsedTime();
    show_restart_text_ = !show_restart_text_;
  }

  if (game_finished_) {
    return;
  }

  world_.Step(
    timestep_,
    velocity_iterations_,
    position_iterations_
  );

  if (holding_) {
    hold_time_ = clock_.getElapsedTime() - shot_start_;
    const int duration = (hold_time_.asMilliseconds() > kMaxHoldTime) ? kMaxHoldTime : hold_time_.asMilliseconds();
    shot_force_ = static_cast<float>(duration) / kMaxHoldTime;
  } else {
    shot_force_ = (shot_force_ > 0.0f) ? (shot_force_ - 0.01f) : 0.0f;
  }

  if (balls_.empty()) {
    end_sound_.play();

    if (p1_scores_ > p2_scores_) {
      p1_win_ = true;
    } else if (p2_scores_ > p1_scores_) {
      p1_win_ = false;
    } else {
      is_draw_ = true;
    }

    game_finished_ = true;
  }

  player_ball_.Update();
  force_bar_.SetPercentage(shot_force_);

  for (int i = 0;i < balls_.size();i++) {
    auto& ball = balls_[i];

    ball->Update();

    if (pool_table_.CheckHole(*ball)) {
      ball->Disable();

      clapping_sound_.play();
      clapping_sound_.setPlayingOffset(sf::seconds(19));

      AddBallToCurrentPlayer(*ball);
      balls_.erase(balls_.begin() + i);
      ball_in_hole_ = true;
      processing_shot_ = false;
    }
  }

  UpdateRayCastData();

  if (IsMovementFinished() && !ball_in_hole_ && processing_shot_) {
    SwitchTurn();
    processing_shot_ = false;
  }

  if (pool_table_.CheckHole(player_ball_)) {
    sad_sound_.play();
    player_ball_.SetPosition(pool_table_.GetCenter());
  }
}

void Game::Render() {
  pool_table_.Draw(window_);

  p1_bar_.Draw(window_);
  p2_bar_.Draw(window_);
  force_bar_.Draw(window_);

  player_ball_.Draw(window_);
  for (const auto& ball : balls_) {
    ball->Draw(window_);
  }

  if (game_finished_) {
    if (is_draw_) {
      win_text_.setString("DRAW");
    } else if (p1_win_) {
      win_text_.setString("P1 WIN");
    } else {
      win_text_.setString("P2 WIN");
    }

    restart_text_.setString("[SPACE] To restart");

    sf::FloatRect text_bounds = win_text_.getLocalBounds();
    win_text_.setOrigin({
      text_bounds.position.x + text_bounds.size.x / 2.0f,
      text_bounds.position.y + text_bounds.size.y / 2.0f
    });
    win_text_.setPosition(pool_table_.GetCenter());
    win_text_.setCharacterSize(80);
    win_text_.setLetterSpacing(1.0f);
    win_text_.setLineSpacing(1.0f);
    win_text_.setStyle(sf::Text::Regular);
    win_text_.setScale({0.5, 0.5});

    text_bounds = restart_text_.getLocalBounds();
    restart_text_.setOrigin({
      text_bounds.position.x + text_bounds.size.x / 2.0f,
      text_bounds.position.y + text_bounds.size.y / 2.0f - 45
    });
    restart_text_.setPosition(pool_table_.GetCenter());
    restart_text_.setCharacterSize(40);
    restart_text_.setLetterSpacing(1.0f);
    restart_text_.setLineSpacing(1.0f);
    restart_text_.setStyle(sf::Text::Regular);
    restart_text_.setScale({0.5, 0.5});

    window_.draw(win_text_);

    if (show_restart_text_) {
      window_.draw(restart_text_);
    }
  }

  const auto ball_position = player_ball_.GetPosition();

  if (!player_ball_.IsMoving()) {
    b2Vec2 start_point(ball_position.x / kScale, ball_position.y / kScale);
    b2Vec2 d(kRayLength * std::cos(shot_angle_) / kScale, kRayLength * std::sin(shot_angle_) / kScale);
    b2Vec2 end_point = start_point + d;

#ifdef DEBUG_DRAW
    callback_.PassDebugDraw(&debug_draw_);
#endif

    world_.RayCast(&callback_, start_point, end_point);

    if (callback_.HaveHit()) {
      sf::VertexArray lines(sf::PrimitiveType::LineStrip, 4);
      lines[0].position = player_ball_.GetPosition();
      lines[1].position = sf::Vector2f(
        callback_.GetHitPoint().x * kScale,
        callback_.GetHitPoint().y * kScale
      );
      lines[2].position = sf::Vector2f(
        callback_.GetHittedBallPosition().x * kScale,
        callback_.GetHittedBallPosition().y * kScale
      );
      auto p = callback_.GetHitPoint() + 100 / kScale * callback_.GetHitNormal();
      lines[3].position = sf::Vector2f(
        p.x * kScale,
        p.y * kScale
      );

      window_.draw(lines);
    } else {
      sf::VertexArray lines(sf::PrimitiveType::LineStrip, 2);
      lines[0].position = player_ball_.GetPosition();
      lines[1].position = sf::Vector2f(
        callback_.GetHitPoint().x * kScale,
        callback_.GetHitPoint().y * kScale
      );

      window_.draw(lines);
    }

#ifdef DEBUG_DRAW
    if (callback_.HaveHit()) {
      debug_draw_.DrawPoint(callback_.GetHitPoint(), 0.1f, b2Color(0.4f, 0.9f, 0.4f));
      debug_draw_.DrawSegment(start_point, callback_.GetHitPoint(), b2Color(0.8f, 0.8f, 0.8f));
      b2Vec2 head = callback_.GetHitPoint() + kRayCastLength / kScale * callback_.GetHitNormal();
      debug_draw_.DrawSegment(callback_.GetHitPoint(), head, b2Color(0.9f, 0.9f, 0.4f));
    } else {
      debug_draw_.DrawSegment(start_point, end_point, b2Color(0.8f, 0.8f, 0.8f));
    }
#endif
  }

#ifdef DEBUG_DRAW
  // world_.DebugDraw();
#endif
}

void Game::LoadFromJSON() {
  std::ifstream config_file("configs/game.json");
  if (!config_file.is_open()) {
    std::cerr << "Failed to open config file" << std::endl;
    return;
  }

  json data = json::parse(config_file);

  config_file.close();

  const auto player_ball_offset = sf::Vector2f(
    data["player_ball"]["x"].get<float>(),
    data["player_ball"]["y"].get<float>()
  );
  player_ball_.SetPosition(pool_table_.GetCenter() + player_ball_offset);

  balls_offset_.x = data["balls"]["x"].get<float>();
  balls_offset_.y = data["balls"]["y"].get<float>();

  force_coefficient_ = data["force_coefficient"].get<float>();

  p1_bar_.SetPosition(
    data["p1_bar"]["x"].get<float>(),
    data["p1_bar"]["y"].get<float>()
  );
  p2_bar_.SetPosition(
    data["p2_bar"]["x"].get<float>(),
    data["p2_bar"]["y"].get<float>()
  );
  force_bar_.SetPosition(
    data["force_bar"]["x"].get<float>(),
    data["force_bar"]["y"].get<float>()
  );

  music_.setVolume(data["music_volume"].get<float>());
  clapping_sound_.setVolume(data["clapping_volume"].get<float>());

  timestep_ = data["physics"]["time_step"].get<float>();
  velocity_iterations_ = data["physics"]["velocity_iterations"].get<int>();
  position_iterations_ = data["physics"]["position_iterations"].get<int>();
}

bool Game::IsMovementFinished() const {
  const bool processing_finished = std::ranges::all_of(balls_, [](auto& ball) {
    return !ball->IsMoving();
  });
  return processing_finished && !player_ball_.IsMoving();
}

void Game::PlaceBalls() {
  const auto table_center = pool_table_.GetCenter();

  balls_.clear();

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < i + 1; j++) {
      auto position = sf::Vector2f(
        i * 20 + table_center.x / 2,
        j * 20 + table_center.y - i * 10
      );

      position += balls_offset_;

      balls_.push_back(std::make_unique<Ball>(world_, position));
      balls_.back()->SetColor(static_cast<BallColor>((i + j) % kColorsCount));
    }
  }

  return;

  balls_.push_back(std::make_unique<Ball>(world_, sf::Vector2f(table_center.x - 100, table_center.y)));
  balls_.back()->SetColor(BallColor::Purple);

  // balls_.push_back(std::make_unique<Ball>(world_, sf::Vector2f(table_center.x - 100, table_center.y - 40)));
  // balls_.back()->SetColor(BallColor::Green);
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

void Game::UpdateRayCastData() {
  callback_.Reset();

  for (const auto& ball : balls_) {
    callback_.AddBall(ball->GetPosition().x, ball->GetPosition().y);
  }

  callback_.SetAngle(shot_angle_);
  callback_.SetBallPosition(
    player_ball_.GetPosition().x,
    player_ball_.GetPosition().y
  );
}

void Game::ResetGame() {
  LoadFromJSON();
  PlaceBalls();
  game_finished_ = false;
  current_turn_ = PlayerNumber::Player1;
  show_restart_text_ = false;
}
