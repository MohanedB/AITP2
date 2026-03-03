#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "../FSM/StateMachine.h"

class HUD {
private:
    sf::Font font;
    sf::Text titleText;
    sf::Text fsmStateText;
    sf::Text fpsText;
    sf::Text instructionText;
    sf::RectangleShape sidebarBg;
    bool fontLoaded;

public:
    HUD();
    
    void Update(float deltaTime, StateMachine& state);
    void Draw(sf::RenderWindow& window);
};
