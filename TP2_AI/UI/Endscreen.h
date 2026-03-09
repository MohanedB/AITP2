#pragma once
#include <SFML/Graphics.hpp>

enum class EndResult { Captured, Escaped };

class EndScreen
{
public:
    EndScreen(sf::Vector2u windowSize);

    void Show(EndResult result);
    void Draw(sf::RenderWindow& window);

    bool HandleEvent(const sf::Event& event, sf::RenderWindow& window);

private:
    sf::RectangleShape overlay;
    sf::RectangleShape retryBtn;
    sf::Font           font;
    sf::Text           titleText;
    sf::Text           subtitleText;
    sf::Text           retryText;
    bool               fontLoaded = false;
    sf::Vector2u       windowSize;

    void Layout();
};