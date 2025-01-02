#include <iostream>
#include <SFML/Graphics.hpp>
#include <optional>

#include "Game.h"
#include "utils/TextureManager.h"

int main() {
  auto window = sf::RenderWindow(sf::VideoMode({640, 360}), "Snooker");
  window.setFramerateLimit(144);

  auto& texture_manager = TextureManager::GetInstance();

  if (!texture_manager.LoadFromJSON("configs/textures.json")) {
    return -1;
  }

  Game game(window);

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }

      if (event.has_value()) {
        game.HandleEvents(event.value());
      }
    }

    window.clear();

    game.Update(0.0f);
    game.Render();

    window.display();
  }

  return 0;
}
