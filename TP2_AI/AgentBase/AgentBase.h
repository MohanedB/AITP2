#pragma once
#include <SFML/Graphics.hpp>

#include "../FSM/StateMachine.h"
#include "../Map/Grid.h"
#include "../AI/Pathfinder.h"

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
    void Draw(sf::RenderWindow& window) const;

    void SetPlayerPosition(sf::Vector2f position);
    sf::Vector2f GetPlayerPosition() const;

    sf::Vector2f GetPosition() const;
    
    StateMachine& GetEnnemyState();

    static RayHit CastRay(Grid& grid, sf::Vector2f origin, sf::Vector2f dir, float maxDist);
    void RayCast(sf::RenderWindow& window, Grid& map, float mapScale) const;

    void SetPatrolPoints(Grid& grid);
    
private:
    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f playerPosition;
    float speed;
    float radius;
    sf::Vector2f patrolPoints[11];
    StateMachine ennemyState = StateMachine();

    float facingAngle = 270.0f;
    float FOV = 60.0f;
    int rayCount = 60;
    int currentPatrolPoint = 0;

    //Mouvement dans Update
    sf::Vector2f target;
    sf::Vector2f movement;
    sf::Vector2f nextPosX;
    sf::Vector2f nextPosY;

    //Pathfinding
    std::vector<sf::Vector2f> currentPath;
    int pathIndex = 0;
};