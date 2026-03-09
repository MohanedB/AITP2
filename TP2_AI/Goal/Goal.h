#pragma once
#include <SFML/Graphics.hpp>
#include "../Map/Grid.h"

class Goal
{
public:
    Goal(sf::Vector2f startPos);
    
    void Draw(sf::RenderWindow& window) const;
    
    sf::Vector2f GetPosition() const;
    
private:
    sf::CircleShape shape;
    sf::Vector2f position;
    float radius;
};
