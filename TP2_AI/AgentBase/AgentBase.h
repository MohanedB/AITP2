#pragma once
#include <SFML/Graphics.hpp>

#include "../FSM/StateMachine.h"
#include "../Map/Grid.h"

struct RayHit
{
    bool hit;
    sf::Vector2f point;
    float distance;
};

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

    RayHit CastRay(Grid& grid, sf::Vector2f origin, sf::Vector2f dir, float maxDist);
    void RayCast(sf::RenderWindow& window, Grid& map, float mapScale);

    void SetPatrolPoints();
    
private:
    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f playerPosition;
    float speed;
    float radius;
    sf::Vector2f patrolPoints[11];
    StateMachine ennemyState = StateMachine();

    float facingAngle = 0.0f;
    float FOV = 60.0f;
    int rayCount = 60;
};