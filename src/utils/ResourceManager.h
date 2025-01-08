#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>

const sf::Texture kDummyTexture;
const sf::SoundBuffer kDummySoundBuffer;

sf::Texture LoadTextureHandler(const std::string& path);
sf::SoundBuffer LoadSoundHandler(const std::string& path);

class AbstractResourceStorage {
public:
  virtual ~AbstractResourceStorage() = default;

  virtual bool Load(const std::string& path, const std::string& name) = 0;
  [[nodiscard]] virtual bool IsLoaded(const std::string& name) const = 0;
};

template <typename T>
class ResourceStorage : public AbstractResourceStorage {
  using LoadHandler = std::function<T(const std::string&)>;

public:
  ResourceStorage() = default;
  ~ResourceStorage() override = default;
  ResourceStorage(const ResourceStorage&) = delete;
  ResourceStorage& operator=(const ResourceStorage&) = delete;

  void SetLoadHandler(const LoadHandler& loadHandler) {
    load_handler_ = loadHandler;
  }

  [[nodiscard]] bool IsLoaded(const std::string& name) const override {
    return resources_.contains(name);
  }

  bool Load(const std::string& path, const std::string& name) override {
    if (IsLoaded(name)) {
      return false;
    }

    resources_[name] = load_handler_(path);

    return true;
  }

  [[nodiscard]] const T& Get(const std::string& name) {
    if (!IsLoaded(name)) {
      return T();
    }

    return resources_[name];
  }

  const T& operator[](const std::string& name) {
    return Get(name);
  }

private:
  LoadHandler load_handler_ = [](const std::string&) {
    std::cerr << "Undefined loaded handler" << std::endl;

    return T();
  };

  std::unordered_map<std::string, T> resources_ {};
};

class ResourceManager {
public:
  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

  static ResourceManager& GetInstance();
  static const sf::Texture& GetTexture(const std::string& name);
  static const sf::SoundBuffer& GetSound(const std::string& name);

  bool LoadTexturesFromJSON(const std::string& path);
  bool LoadSoundsFromJSON(const std::string& path);

  [[nodiscard]] bool IsLoaded(const std::string& name) const;

private:
  ResourceManager();

  bool LoadFromJSON(const std::string& path, AbstractResourceStorage& storage);

  ResourceStorage<sf::Texture> textures_storage_;
  ResourceStorage<sf::SoundBuffer> sound_storage_;
};
