#pragma once
#include <SFML/Graphics.hpp>

#include "../FSM/StateMachine.h"
#include "../Map/Grid.h"

class AgentBase
{
public:
    AgentBase(sf::Vector2f startPos);
    
    void Update(float deltaTime, Grid& grid);
    void Draw(sf::RenderWindow& window);

    void SetPlayerPosition(sf::Vector2f position);
    sf::Vector2f GetPlayerPosition();

    sf::Vector2f GetPosition();
    
    StateMachine& GetEnnemyState();
    
private:
    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f playerPosition;
    float speed;
    float radius;
    sf::Vector2f patrolPoints[11];
    StateMachine ennemyState = StateMachine();
};
