// Menu.h
#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Menu {
public:
    Menu(sf::RenderWindow& window) : window(window) {
        if (!font.loadFromFile("assets/fonts/arial.ttf")) {
            // Обработка ошибки загрузки шрифта
        }

        title.setString("Lamp Chess 10x10");
        title.setFont(font);
        title.setCharacterSize(60);
        title.setPosition(200.0f, 100.0f);

        std::string labels[3] = { "Play vs Friend", "Play vs AI", "Exit" };
        for (int i = 0; i < 3; ++i) {
            buttons[i].setSize(sf::Vector2f(400.0f, 80.0f));
            buttons[i].setPosition(300.0f, 250.0f + i * 120.0f);
            buttons[i].setFillColor(sf::Color(70, 70, 70));

            buttonTexts[i].setString(labels[i]);
            buttonTexts[i].setFont(font);
            buttonTexts[i].setCharacterSize(30);
            float textWidth = buttonTexts[i].getLocalBounds().width;
            buttonTexts[i].setPosition(
                300.0f + (400.0f - textWidth) / 2.0f,
                250.0f + i * 120.0f + 20.0f
            );
        }

        buttons[3].setSize(sf::Vector2f(400.0f, 80.0f));
        buttons[3].setPosition(300.0f, 250.0f + 3 * 120.0f); // Ниже остальных
        buttons[3].setFillColor(sf::Color(100, 0, 0)); // Темно-красный цвет

        buttonTexts[3].setString("Hard Mode");
        buttonTexts[3].setFont(font);
        buttonTexts[3].setCharacterSize(30);
        float textWidth = buttonTexts[3].getLocalBounds().width;
        buttonTexts[3].setPosition(
            300.0f + (400.0f - textWidth) / 2.0f,
            250.0f + 3 * 120.0f + 20.0f
        );
    }

    void draw() {
        window.draw(title);
        for (int i = 0; i < 4; ++i) {
            window.draw(buttons[i]);
            window.draw(buttonTexts[i]);
        }
    }

    int handleClick(const sf::Vector2i& mousePos) {
        for (int i = 0; i < 4; ++i) { // Проверяем 4 кнопки
            if (buttons[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                return i + 1; // 1-Friend, 2-AI, 3-Exit, 4-Hard
            }
        }
        return 0;
    }

private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text title;
    sf::RectangleShape buttons[4];
    sf::Text buttonTexts[4];
};