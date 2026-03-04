#pragma once
#include <SFML/Graphics.hpp>
#include "../Map/Grid.h"

class AgentBase
{
private:
    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f playerPosition;
    float speed;
    float radius;

public:
    AgentBase(sf::Vector2f startPos);
    
    void Update(float deltaTime, Grid& grid);
    void Draw(sf::RenderWindow& window);

    void SetPlayerPosition(sf::Vector2f position) const;
    sf::Vector2f GetPlayerPosition() const;
};
