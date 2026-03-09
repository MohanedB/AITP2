#include "AgentBase.h"
#include <cmath>
#include <cstdlib>

// ------------------------------------------------------------------ helpers
float VecLength(sf::Vector2f v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float VecDist(sf::Vector2f a, sf::Vector2f b)
{
    return VecLength(b - a);
}

// ------------------------------------------------------------------ constructeur
AgentBase::AgentBase(sf::Vector2f startPos)
{
    position        = startPos;
    speed           = 80.0f;
    radius          = 8.0f;
    captureDistance = 14.0f;
    arriveRadius    = 60.0f;
    wanderDist      = 80.0f;
    wanderJitter    = 40.0f;
    wanderAngle     = static_cast<float>(std::rand() % 360);
    facingAngle     = 270.0f;
    FOV             = 60.0f;
    rayCount        = 60;
    currentPatrolPoint = 0;
    pathIndex       = 0;
    pathCooldown    = 0.0f;
    playerVisible   = false;

    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin({ radius, radius });
    shape.setPosition(position);
}

// ------------------------------------------------------------------ getters
void AgentBase::Draw(sf::RenderWindow& window)
{
    window.draw(shape);
}

void AgentBase::SetPlayerPosition(sf::Vector2f pos)
{
    playerPosition = pos;
}

sf::Vector2f AgentBase::GetPlayerPosition()
{
    return playerPosition;
}

sf::Vector2f AgentBase::GetPosition()
{
    return position;
}

StateMachine& AgentBase::GetEnnemyStateMachine()
{
    return ennemyState;
}

bool AgentBase::HasCapturedPlayer()
{
    return VecDist(position, playerPosition) < captureDistance;
}

// ================================================================== STEERING BEHAVIORS

// Seek : va vers la cible a vitesse max
sf::Vector2f AgentBase::Seek(sf::Vector2f target)
{
    sf::Vector2f direction = target - position;
    float length = VecLength(direction);

    if (length < 0.001f)
        return sf::Vector2f(0, 0);

    direction = direction / length; // normaliser
    return direction * speed;
}

// Arrive : comme Seek mais ralentit quand on est proche
sf::Vector2f AgentBase::Arrive(sf::Vector2f target)
{
    sf::Vector2f direction = target - position;
    float dist = VecLength(direction);

    if (dist < 0.001f)
        return sf::Vector2f(0, 0);

    float desiredSpeed = speed;
    if (dist < arriveRadius)
        desiredSpeed = speed * (dist / arriveRadius);

    direction = direction / dist;
    return direction * desiredSpeed;
}

// Wander : mouvement aleatoire fluide
sf::Vector2f AgentBase::Wander(float deltaTime)
{
    float jitter = wanderJitter * deltaTime;
    float randomValue = static_cast<float>(std::rand()) / RAND_MAX;
    wanderAngle += (randomValue * 2.0f - 1.0f) * jitter;

    float PI = 3.14159265f;

    sf::Vector2f facing;
    facing.x = std::cos(facingAngle * PI / 180.f);
    facing.y = std::sin(facingAngle * PI / 180.f);

    sf::Vector2f circleCenter = position + facing * wanderDist;

    sf::Vector2f wanderPoint;
    wanderPoint.x = circleCenter.x + std::cos(wanderAngle) * 30.0f;
    wanderPoint.y = circleCenter.y + std::sin(wanderAngle) * 30.0f;

    return Seek(wanderPoint);
}

// ================================================================== COLLISION
bool AgentBase::IsWall(Grid& grid, float px, float py)
{
    int gx = static_cast<int>(px / grid.getTileSize());
    int gy = static_cast<int>(py / grid.getTileSize());
    Node* node = grid.getNode(gx, gy);
    return (node == nullptr || node->isObstacle);
}

// ================================================================== APPLY VELOCITY
void AgentBase::ApplyVelocity(sf::Vector2f velocity, float deltaTime, Grid& grid)
{
    float len = VecLength(velocity);
    if (len < 0.001f)
        return;

    float PI = 3.14159265f;
    facingAngle = std::atan2(velocity.y, velocity.x) * 180.f / PI;

    float r  = radius - 1.0f;
    float dx = velocity.x * deltaTime;
    float dy = velocity.y * deltaTime;

    // Collision axe X
    sf::Vector2f nextX = position;
    nextX.x += dx;
    if (!IsWall(grid, nextX.x - r, position.y - r) &&
        !IsWall(grid, nextX.x + r, position.y - r) &&
        !IsWall(grid, nextX.x - r, position.y + r) &&
        !IsWall(grid, nextX.x + r, position.y + r))
    {
        position.x = nextX.x;
    }

    // Collision axe Y
    sf::Vector2f nextY = position;
    nextY.y += dy;
    if (!IsWall(grid, position.x - r, nextY.y - r) &&
        !IsWall(grid, position.x + r, nextY.y - r) &&
        !IsWall(grid, position.x - r, nextY.y + r) &&
        !IsWall(grid, position.x + r, nextY.y + r))
    {
        position.y = nextY.y;
    }

    shape.setPosition(position);
}

// ================================================================== PATHFINDING
void AgentBase::RequestPath(Grid& grid, sf::Vector2f target)
{
    path = Pathfinder::FindPath(grid, position, target);
    pathIndex = 0;
}

void AgentBase::FollowPath(float deltaTime, Grid& grid, bool useArrive)
{
    // Passer les waypoints deja atteints
    while (pathIndex < (int)path.size() && VecDist(position, path[pathIndex]) < 6.0f)
        pathIndex++;

    if (path.empty() || pathIndex >= (int)path.size())
        return;

    sf::Vector2f waypoint = path[pathIndex];
    bool isLastWaypoint = (pathIndex == (int)path.size() - 1);

    sf::Vector2f velocity;
    if (useArrive && isLastWaypoint)
        velocity = Arrive(waypoint);
    else
        velocity = Seek(waypoint);

    ApplyVelocity(velocity, deltaTime, grid);
}

// ================================================================== UPDATE (FSM)
void AgentBase::Update(float deltaTime, Grid& grid)
{
    if (pathCooldown > 0.0f)
        pathCooldown -= deltaTime;

    States currentState = ennemyState.GetState();

    // Detection : FOV en patrouille, LOS seul en poursuite/retour
    bool useFOV = (currentState == States::Patrouille);
    playerVisible = CanSeePlayer(grid, useFOV);

    // ---- Transitions FSM ----
    if (currentState == States::Patrouille)
    {
        if (playerVisible)
        {
            ennemyState.SetState(States::Poursuite);
            RequestPath(grid, playerPosition);
            pathCooldown = 0.5f;
        }
    }
    else if (currentState == States::Poursuite)
    {
        if (!playerVisible)
        {
            ennemyState.SetState(States::Retour);
            RequestPath(grid, patrolPoints[currentPatrolPoint]);
        }
        else if (pathCooldown <= 0.0f)
        {
            RequestPath(grid, playerPosition);
            pathCooldown = 0.5f;
        }
    }
    else if (currentState == States::Retour)
    {
        if (playerVisible)
        {
            ennemyState.SetState(States::Poursuite);
            RequestPath(grid, playerPosition);
            pathCooldown = 0.5f;
        }
        else if (path.empty() || pathIndex >= (int)path.size())
        {
            ennemyState.SetState(States::Patrouille);
            currentPatrolPoint = (currentPatrolPoint + 1) % PATROL_COUNT;
            RequestPath(grid, patrolPoints[currentPatrolPoint]);
        }
    }

    // ---- Comportements (steering) ----
    States activeState = ennemyState.GetState();

    if (activeState == States::Patrouille)
    {
        if (path.empty() || pathIndex >= (int)path.size())
        {
            currentPatrolPoint = (currentPatrolPoint + 1) % PATROL_COUNT;
            RequestPath(grid, patrolPoints[currentPatrolPoint]);
        }
        FollowPath(deltaTime, grid, false);
    }
    else if (activeState == States::Poursuite)
    {
        FollowPath(deltaTime, grid, false);
    }
    else if (activeState == States::Retour)
    {
        FollowPath(deltaTime, grid, true);
    }
}

// ================================================================== DETECTION
bool AgentBase::CanSeePlayer(Grid& grid, bool checkFOV)
{
    sf::Vector2f toPlayer = playerPosition - position;
    float dist = VecLength(toPlayer);

    if (dist < 0.001f || dist > 300.0f)
        return false;

    float PI = 3.14159265f;

    if (checkFOV)
    {
        float angleToPlayer = std::atan2(toPlayer.y, toPlayer.x) * 180.f / PI;
        float diff = angleToPlayer - facingAngle;

        while (diff >  180.f) diff -= 360.f;
        while (diff < -180.f) diff += 360.f;

        if (std::abs(diff) > FOV / 2.0f)
            return false;
    }

    sf::Vector2f dir = toPlayer / dist;
    RayHit hit = CastRay(grid, position, dir, dist);
    return !hit.hit;
}

// ================================================================== RAYCAST
RayHit AgentBase::CastRay(Grid& grid, sf::Vector2f origin, sf::Vector2f dir, float maxDist)
{
    float len = VecLength(dir);
    if (len < 0.001f)
        return { false, origin, 0 };

    dir = dir / len;

    sf::Vector2f pos = origin;
    float traveled = 0.0f;
    float step = 2.0f;

    while (traveled < maxDist)
    {
        pos += dir * step;
        traveled += step;

        int gx = static_cast<int>(pos.x / grid.getTileSize());
        int gy = static_cast<int>(pos.y / grid.getTileSize());
        Node* node = grid.getNode(gx, gy);

        if (node == nullptr || node->isObstacle)
            return { true, pos, traveled };
    }

    return { false, pos, maxDist };
}

void AgentBase::RayCast(sf::RenderWindow& window, Grid& grid, float mapScale)
{
    float PI = 3.14159265f;
    float angleStart = facingAngle - FOV / 2.0f;

    sf::Color rayColor;
    if (playerVisible)
        rayColor = sf::Color::Red;
    else
        rayColor = sf::Color::White;

    for (int i = 0; i < rayCount; i++)
    {
        float angle = angleStart + (FOV / (float)rayCount) * i;

        sf::Vector2f dir;
        dir.x = std::cos(angle * PI / 180.f);
        dir.y = std::sin(angle * PI / 180.f);

        RayHit hit = CastRay(grid, position, dir, 300.0f);

        sf::Vertex line[2];
        line[0].position = position * mapScale;
        line[0].color    = rayColor;
        line[1].position = hit.point * mapScale;
        line[1].color    = rayColor;

        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
}

// ================================================================== PATROL POINTS
void AgentBase::SetPatrolPoints()
{
    patrolPoints[0] = position;

    for (int i = 1; i < PATROL_COUNT; i++)
    {
        sf::Vector2f prev = patrolPoints[i - 1];

        if (prev.x < 725.0f && prev.y < 545.0f)
            patrolPoints[i] = sf::Vector2f(prev.x + 45.0f, prev.y + 25.0f);
        else
            patrolPoints[i] = sf::Vector2f(prev.x - 325.0f, prev.y - 270.0f);
    }
}