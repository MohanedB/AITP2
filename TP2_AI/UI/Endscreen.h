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
    sf::Vector2u windowSize;
    sf::Font font;
    bool fontLoaded;

    sf::RectangleShape overlay;
    sf::RectangleShape retryBtn;
    sf::Text titleText;
    sf::Text subtitleText;
    sf::Text retryText;
};