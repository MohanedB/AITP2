#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
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
    void Draw(sf::RenderWindow& window);

    void SetPlayerPosition(sf::Vector2f pos);
    sf::Vector2f GetPlayerPosition();
    sf::Vector2f GetPosition();

    StateMachine& GetEnnemyState();
    bool HasCapturedPlayer();

    RayHit CastRay(Grid& grid, sf::Vector2f origin, sf::Vector2f dir, float maxDist);
    void RayCast(sf::RenderWindow& window, Grid& grid, float mapScale);

    void SetPatrolPoints();

private:
    sf::Vector2f Seek(sf::Vector2f target);
    sf::Vector2f Arrive(sf::Vector2f target);
    sf::Vector2f Wander(float deltaTime);

    bool IsWall(Grid& grid, float px, float py);
    void ApplyVelocity(sf::Vector2f velocity, float deltaTime, Grid& grid);

    void RequestPath(Grid& grid, sf::Vector2f target);
    void FollowPath(float deltaTime, Grid& grid, bool useArrive);

    bool CanSeePlayer(Grid& grid, bool checkFOV);

    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f playerPosition;

    float speed;
    float radius;
    float captureDistance;
    float arriveRadius;
    float wanderDist;
    float wanderJitter;
    float wanderAngle;
    float facingAngle;
    float FOV;
    int rayCount;

    static constexpr int PATROL_COUNT = 11;
    sf::Vector2f patrolPoints[PATROL_COUNT];
    int currentPatrolPoint;

    std::vector<sf::Vector2f> path;
    int pathIndex;
    float pathCooldown;

    bool playerVisible;
    StateMachine ennemyState;
};