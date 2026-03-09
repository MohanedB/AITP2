#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../FSM/StateMachine.h"

class HUD
{
private:
    sf::Font font;
    sf::Text titleText;
    sf::Text fpsText;
    sf::Text instructionText;
    sf::RectangleShape sidebarBg;
    bool fontLoaded;

    // Un sf::Text par ennemi
    std::vector<sf::Text> enemyStateTexts;

public:
    HUD();

    void Update(float deltaTime, std::vector<StateMachine*> states);
    void Draw(sf::RenderWindow& window);
};