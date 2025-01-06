#pragma once

 #include <SFML/Graphics.hpp>
 #include <unordered_map>

const sf::Texture kDummyTexture;

class TextureManager {
public:
   TextureManager(const TextureManager&) = delete;
   TextureManager& operator=(const TextureManager&) = delete;

   static TextureManager& GetInstance();

   bool LoadFromJSON(const std::string& path);

   bool Load(const std::string& path, const std::string& name);
   [[nodiscard]] std::shared_ptr<sf::Texture> GetTexture(const std::string& name) const;
   [[nodiscard]] bool IsLoaded(const std::string& name) const;

  private:
   TextureManager() = default;

   std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures {};
};
