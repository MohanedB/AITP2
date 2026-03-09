#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "../FSM/StateMachine.h"
#include "../Map/Grid.h"
#include "../AI/Pathfinder.h"

struct RayHit
{
    bool         hit;
    sf::Vector2f point;
    float        distance;
};

class AgentBase
{
public:
    AgentBase(sf::Vector2f startPos);

    void Update(float deltaTime, Grid& grid);
    void Draw(sf::RenderWindow& window);

    void         SetPlayerPosition(sf::Vector2f pos);
    sf::Vector2f GetPlayerPosition() const;
    sf::Vector2f GetPosition()       const;

    StateMachine& GetEnnemyState();

    bool HasCapturedPlayer() const; // fin de partie

    RayHit CastRay(Grid& grid, sf::Vector2f origin, sf::Vector2f dir, float maxDist);
    void   RayCast(sf::RenderWindow& window, Grid& grid, float mapScale);

    void SetPatrolPoints();

private:
    // --- Steering behaviors ---
    // Retournent une velocite desirée (normalisee * speed)
    sf::Vector2f Seek(sf::Vector2f target);
    sf::Vector2f Arrive(sf::Vector2f target, float slowRadius = 60.0f);
    sf::Vector2f Wander(float deltaTime);

    // Applique la velocite avec collision
    void ApplyVelocity(sf::Vector2f velocity, float deltaTime, Grid& grid);

    // --- Navigation ---
    void         RequestPath(Grid& grid, sf::Vector2f target);
    void         FollowPath(float deltaTime, Grid& grid, bool useArrive = false);
    sf::Vector2f GetLookaheadTarget() const;

    // --- Detection ---
    bool CanSeePlayer(Grid& grid, bool withFOV) const;

    // --- Data ---
    sf::CircleShape shape;
    sf::Vector2f    position;
    sf::Vector2f    playerPosition;
    float           speed;
    float           radius;

    static constexpr int PATROL_COUNT = 11;

    float captureDistance; // distance de capture
    float arriveRadius;    // commence a ralentir a cette distance
    float wanderDist;      // rayon du cercle de wander
    float wanderJitter;    // variation max par frame

    sf::Vector2f patrolPoints[PATROL_COUNT];
    int          currentPatrolPoint = 0;

    std::vector<sf::Vector2f> path;
    int   pathIndex    = 0;
    float pathCooldown = 0.0f;

    // Wander state
    float wanderAngle = 0.0f;

    StateMachine ennemyState;
    bool         playerVisible = false;

    float facingAngle = 270.0f;
    float FOV         = 60.0f;
    int   rayCount    = 60;
};