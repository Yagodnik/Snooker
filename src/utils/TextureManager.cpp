#include "TextureManager.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

TextureManager& TextureManager::GetInstance() {
  static TextureManager instance;
  return instance;
}

bool TextureManager::LoadFromJSON(const std::string &path) {
  std::ifstream config_file(path);
  if (!config_file.is_open()) {
    std::cerr << "Failed to open config file " << path << std::endl;
    return false;
  }

  json data = json::parse(config_file);

  config_file.close();

  if (!data.is_array()) {
    std::cerr << "Failed to parse config file " << path << std::endl;
    return false;
  }

  for (const auto& texture : data) {
    if (!texture.contains("path")) {
      std::cerr << "Invalid json format!" << std::endl;
      return false;
    }

    if (!texture.contains("name")) {
      std::cerr << "Invalid json format!" << std::endl;
      return false;
    }

    Load(texture["path"], texture["name"]);
  }

  return true;
}

bool TextureManager::Load(const std::string& path, const std::string& name) {
  const auto texture = std::make_shared<sf::Texture>();

  if (!texture->loadFromFile(path)) {
    return false;
  }

  textures[name] = texture;

  return true;
}

std::shared_ptr<sf::Texture> TextureManager::GetTexture(const std::string &name) const {
  return textures.at(name);
}

bool TextureManager::IsLoaded(const std::string &name) const {
  return textures.contains(name);
}
