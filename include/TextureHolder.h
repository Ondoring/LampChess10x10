#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <filesystem>

class TextureHolder {
private:
    std::map<std::string, sf::Texture> textures;
public:
    void load(const std::string& name, const std::string& filename) {
        if (!textures[name].loadFromFile(filename)) {
            throw std::runtime_error("Failed to load texture: " + filename +
                "\nWD: " + std::filesystem::current_path().string());
        }
    }
    sf::Texture& get(const std::string& name) {
        return textures[name];
    }
};
