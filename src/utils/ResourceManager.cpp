#include "ResourceManager.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

ResourceManager::ResourceManager() {
  textures_storage_.SetLoadHandler(LoadTextureHandler);
  sound_storage_.SetLoadHandler(LoadSoundHandler);
}

bool ResourceManager::LoadFromJSON(const std::string &path, AbstractResourceStorage &storage) {
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

    const bool ret = storage.Load(
      texture["path"],
      texture["name"]
    );

    if (!ret) {
      std::cerr << "Already loaded!" << std::endl;
      return false;
    }
  }

  return true;
}

sf::Texture LoadTextureHandler(const std::string &path) {
  sf::Texture texture;
  if (!texture.loadFromFile(path)) {
    std::cerr << "Failed to load texture" << std::endl;
    return kDummyTexture;
  }

  return texture;
}

sf::SoundBuffer LoadSoundHandler(const std::string &path) {
  sf::SoundBuffer buffer;
  if (!buffer.loadFromFile(path)) {
    std::cerr << "Failed to load texture" << std::endl;
    return kDummySoundBuffer;
  }

  return buffer;
}

ResourceManager& ResourceManager::GetInstance() {
  static ResourceManager instance;
  return instance;
}

const sf::Texture& ResourceManager::GetTexture(const std::string& name) {
  ResourceManager& instance = GetInstance();

  return instance.textures_storage_.Get(name);
}

const sf::SoundBuffer & ResourceManager::GetSound(const std::string &name) {
  ResourceManager& instance = GetInstance();

  return instance.sound_storage_.Get(name);
}

bool ResourceManager::LoadTexturesFromJSON(const std::string &path) {
  return LoadFromJSON(path, textures_storage_);
}

bool ResourceManager::LoadSoundsFromJSON(const std::string &path) {
  return LoadFromJSON(path, sound_storage_);
}

bool ResourceManager::IsLoaded(const std::string &name) const {
  return textures_storage_.IsLoaded(name);
}
