#include <iostream>
#include <SFML/Graphics.hpp>
#include <optional>

#include "Game.h"
#include "utils/TextureManager.h"

int main() {
  auto window = sf::RenderWindow(
    sf::VideoMode({640, 360}),
    "Snooker"
  );
  window.setFramerateLimit(144);
  const float aspect_ratio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);

  std::cout << "Aspect ratio: " << aspect_ratio << std::endl;

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

      // if (const auto& resized_event = event->getIf<sf::Event::Resized>()) {
      //   window.setSize({resized_event->size.x, static_cast<unsigned>(resized_event->size.x / aspect_ratio)});
      // }

      game.HandleEvents(event);
    }

    window.clear();

    game.Update(0.0f);
    game.Render();

    window.display();
  }

  return 0;
}
