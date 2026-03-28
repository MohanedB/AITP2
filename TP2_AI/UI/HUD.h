#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class HUD {
public:
    HUD();

    void Update(float dt,
                const std::vector<std::string>& agentGoals,
                bool playerHasKey,
                bool nearExitNoKey,
                bool alertActive);

    void Draw(sf::RenderWindow& window);

private:
    sf::Font font;
    bool fontLoaded;

    sf::RectangleShape sidebarBg;
    sf::Text titleText;
    sf::Text legendText;
    sf::Text keyStatusText;
    sf::Text alertStatusText;
    sf::Text fpsText;
    sf::Text instructionText;

    std::vector<sf::Text> agentTexts;

    // Message flash (centre écran)
    sf::Text  flashText;
    float     flashTimer   = 0.0f;
    bool      prevHasKey   = false;
    bool      prevNearExit = false;

    void SetFlash(const std::string& msg, sf::Color col, float duration);

    static sf::Color ColorForGoal(const std::string& goal);
};