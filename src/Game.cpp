#include "Game.h"

Game::Game(sf::RenderWindow& window) :
  window_(window),
  world_({0.f, 0.f}),
  p1_bar_(PlayerNumber::Player1),
  p2_bar_(PlayerNumber::Player2),
  pool_table_(world_),
  player_ball_(world_, pool_table_.GetCenter()),
  current_turn_(PlayerNumber::Player1),
  music_("assets/sounds/snooker_music.mp3"),
  clapping_sound_buffer_("assets/sounds/clapping.mp3"),
  clapping_sound_(clapping_sound_buffer_),
  sad_sound_buffer_("assets/sounds/sad_sound.mp3"),
  sad_sound_(sad_sound_buffer_),
  game_finished_(false),
  is_draw_(false),
  p1_win_(false),
  pixel_font_("assets/fonts/Jersey25-Regular.ttf"),
  // pixel_font_("assets/fonts/Geologica-Regular.ttf"),
  text_(pixel_font_),
  debug_draw_(window_, 30.f)
{
  world_.SetContactListener(&contact_listener_);
  world_.SetDebugDraw(&debug_draw_);
  debug_draw_.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit);

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
  if (game_finished_) {
    return;
  }

  if (holding_) {
    hold_time_ = clock_.getElapsedTime() - shot_start_;
    const int duration = (hold_time_.asMilliseconds() > kMaxHoldTime) ? kMaxHoldTime : hold_time_.asMilliseconds();
    shot_force_ = static_cast<float>(duration) / kMaxHoldTime;
  } else {
    shot_force_ = (shot_force_ > 0.0f) ? (shot_force_ - 0.01f) : 0.0f;
  }

  if (balls_.empty()) {
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

  bool processing_finished = std::ranges::all_of(balls_, [](auto& ball) {
    return !ball->IsMoving();
  });
  processing_finished = processing_finished && !player_ball_.IsMoving();

  if (processing_finished && !ball_in_hole_ && processing_shot_) {
    SwitchTurn();
    processing_shot_ = false;
  }

  if (pool_table_.CheckHole(player_ball_)) {
    sad_sound_.play();
    player_ball_.SetPosition(pool_table_.GetCenter());
  }


  world_.Step(1 / 60.f, 8,3);
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
      text_.setString("DRAW");
    } else if (p1_win_) {
      text_.setString("P1 WIN");
    } else {
      text_.setString("P2 WIN");
    }

    const sf::FloatRect text_bounds = text_.getLocalBounds();
    text_.setOrigin({
      text_bounds.position.x + text_bounds.size.x / 2.0f,
      text_bounds.position.y + text_bounds.size.y / 2.0f
    });
    text_.setPosition(pool_table_.GetCenter());
    text_.setCharacterSize(40);
    text_.setLetterSpacing(1.0f);
    text_.setLineSpacing(1.0f);
    text_.setStyle(sf::Text::Regular);
    text_.setScale({1, 1});

    window_.draw(text_);
  }

#ifdef DEBUG_DRAW
  world_.DebugDraw();
#endif
}

void Game::PlaceBalls() {
  const auto table_center = pool_table_.GetCenter();

  balls_.clear();

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < i + 1; j++) {
      balls_.push_back(std::make_unique<Ball>(world_, sf::Vector2f(i * 20 + table_center.x / 2, j * 20 + table_center.y - i * 10)));
      balls_.back()->SetColor(static_cast<BallColor>((i + j) % kColorsCount));
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
  std::cout << "Turn switched!" << std::endl;
  if (current_turn_ == PlayerNumber::Player1) {
    current_turn_ = PlayerNumber::Player2;
  } else {
    current_turn_ = PlayerNumber::Player1;
  }
}
