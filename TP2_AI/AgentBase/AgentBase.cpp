#include "AgentBase.h"
#include <cmath>
#include <cstdlib>

static float VecLength(sf::Vector2f v) { return std::sqrt(v.x * v.x + v.y * v.y); }
static float VecDist(sf::Vector2f a, sf::Vector2f b) { return VecLength(b - a); }
static constexpr float PI = 3.14159265f;

// ------------------------------------------------------------------ ctor
AgentBase::AgentBase(sf::Vector2f startPos)
    : position(startPos), speed(80.0f), radius(8.0f),
      captureDistance(14.0f), arriveRadius(60.0f),
      wanderDist(80.0f), wanderJitter(40.0f)
{
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin({ radius, radius });
    shape.setPosition(position);

    wanderAngle = static_cast<float>(std::rand() % 360);
}

// ------------------------------------------------------------------ public getters
void AgentBase::Draw(sf::RenderWindow& window)       { window.draw(shape); }
void AgentBase::SetPlayerPosition(sf::Vector2f pos)  { playerPosition = pos; }
sf::Vector2f AgentBase::GetPlayerPosition() const    { return playerPosition; }
sf::Vector2f AgentBase::GetPosition()       const    { return position; }
StateMachine& AgentBase::GetEnnemyState()            { return ennemyState; }

bool AgentBase::HasCapturedPlayer() const
{
    return VecDist(position, playerPosition) < captureDistance;
}

// ================================================================== STEERING BEHAVIORS

sf::Vector2f AgentBase::Seek(sf::Vector2f target)
{
    sf::Vector2f toTarget = target - position;
    float len = VecLength(toTarget);
    if (len < 0.001f) return { 0, 0 };
    return (toTarget / len) * speed;
}

sf::Vector2f AgentBase::Arrive(sf::Vector2f target, float slowRadius)
{
    sf::Vector2f toTarget = target - position;
    float dist = VecLength(toTarget);
    if (dist < 0.001f) return { 0, 0 };

    float desiredSpeed = speed;
    if (dist < slowRadius)
        desiredSpeed = speed * (dist / slowRadius);

    return (toTarget / dist) * desiredSpeed;
}

sf::Vector2f AgentBase::Wander(float deltaTime)
{
    float jitter = wanderJitter * deltaTime;
    wanderAngle += (static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f) * jitter;

    sf::Vector2f facing(std::cos(facingAngle * PI / 180.f),
                        std::sin(facingAngle * PI / 180.f));
    sf::Vector2f circleCenter = position + facing * wanderDist;

    sf::Vector2f wanderPoint(circleCenter.x + std::cos(wanderAngle) * 30.0f,
                             circleCenter.y + std::sin(wanderAngle) * 30.0f);

    return Seek(wanderPoint);
}

// ================================================================== APPLY VELOCITY
void AgentBase::ApplyVelocity(sf::Vector2f velocity, float deltaTime, Grid& grid)
{
    float len = VecLength(velocity);
    if (len < 0.001f) return;

    facingAngle = std::atan2(velocity.y, velocity.x) * 180.f / PI;

    auto isWall = [&](float px, float py) -> bool {
        int gx = static_cast<int>(px / grid.getTileSize());
        int gy = static_cast<int>(py / grid.getTileSize());
        Node* node = grid.getNode(gx, gy);
        return (node == nullptr || node->isObstacle);
    };

    float r  = radius - 1.0f;
    float dx = velocity.x * deltaTime;
    float dy = velocity.y * deltaTime;

    sf::Vector2f nextX = position; nextX.x += dx;
    if (!isWall(nextX.x - r, position.y - r) &&
        !isWall(nextX.x + r, position.y - r) &&
        !isWall(nextX.x - r, position.y + r) &&
        !isWall(nextX.x + r, position.y + r))
        position.x = nextX.x;

    sf::Vector2f nextY = position; nextY.y += dy;
    if (!isWall(position.x - r, nextY.y - r) &&
        !isWall(position.x + r, nextY.y - r) &&
        !isWall(position.x - r, nextY.y + r) &&
        !isWall(position.x + r, nextY.y + r))
        position.y = nextY.y;

    shape.setPosition(position);
}

// ================================================================== PATH NAVIGATION
void AgentBase::RequestPath(Grid& grid, sf::Vector2f target)
{
    path      = Pathfinder::FindPath(grid, position, target);
    pathIndex = 0;
}

sf::Vector2f AgentBase::GetLookaheadTarget() const
{
    if (path.empty() || pathIndex >= (int)path.size())
        return position;
    int lookahead = std::min(pathIndex + 4, (int)path.size() - 1);
    return path[lookahead];
}

void AgentBase::FollowPath(float deltaTime, Grid& grid, bool useArrive)
{
    // Avancer pathIndex tant qu'on est proche du waypoint courant
    while (pathIndex < (int)path.size() &&
           VecDist(position, path[pathIndex]) < 6.0f)
        pathIndex++;

    if (path.empty() || pathIndex >= (int)path.size())
        return;

    sf::Vector2f waypoint = path[pathIndex];
    bool isLastWaypoint   = (pathIndex == (int)path.size() - 1);

    sf::Vector2f velocity = (useArrive && isLastWaypoint)
                            ? Arrive(waypoint, arriveRadius)
                            : Seek(waypoint);

    ApplyVelocity(velocity, deltaTime, grid);
}

// ================================================================== UPDATE (FSM)
void AgentBase::Update(float deltaTime, Grid& grid)
{
    if (pathCooldown > 0.0f)
        pathCooldown -= deltaTime;

    States current = ennemyState.GetState();

    bool usesFOV  = (current == States::Patrouille);
    playerVisible = CanSeePlayer(grid, usesFOV);
    bool playerLost = !playerVisible;

    // --- Transitions FSM ---
    switch (current)
    {
        case States::Patrouille:
            if (playerVisible)
            {
                ennemyState.SetState(States::Poursuite);
                RequestPath(grid, playerPosition);
                pathCooldown = 0.5f;
            }
            break;

        case States::Poursuite:
            if (playerLost)
            {
                ennemyState.SetState(States::Retour);
                RequestPath(grid, patrolPoints[currentPatrolPoint]);
            }
            else if (pathCooldown <= 0.0f)
            {
                RequestPath(grid, playerPosition);
                pathCooldown = 0.5f;
            }
            break;

        case States::Retour:
            if (!playerLost)
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
            break;
    }

    // --- Comportements (steering) ---
    switch (ennemyState.GetState())
    {
        case States::Patrouille:
        {
            if (path.empty() || pathIndex >= (int)path.size())
            {
                currentPatrolPoint = (currentPatrolPoint + 1) % PATROL_COUNT;
                RequestPath(grid, patrolPoints[currentPatrolPoint]);
            }
            FollowPath(deltaTime, grid, false); 
            break;
        }

        case States::Poursuite:
            FollowPath(deltaTime, grid, false);
            break;

        case States::Retour:
            FollowPath(deltaTime, grid, true);
            break;
    }
}

// ================================================================== DETECTION
bool AgentBase::CanSeePlayer(Grid& grid, bool withFOV) const
{
    sf::Vector2f toPlayer = playerPosition - position;
    float dist = VecLength(toPlayer);

    if (dist < 0.001f || dist > 300.0f)
        return false;

    if (withFOV)
    {
        float angleToPlayer = std::atan2(toPlayer.y, toPlayer.x) * 180.f / PI;
        float diff = angleToPlayer - facingAngle;
        while (diff >  180.f) diff -= 360.f;
        while (diff < -180.f) diff += 360.f;
        if (std::abs(diff) > FOV / 2.0f)
            return false;
    }

    sf::Vector2f dir = toPlayer / dist;
    RayHit hit = const_cast<AgentBase*>(this)->CastRay(grid, position, dir, dist);
    return !hit.hit;
}

// ================================================================== RAYCAST
RayHit AgentBase::CastRay(Grid& grid, sf::Vector2f origin, sf::Vector2f dir, float maxDist)
{
    float len = VecLength(dir);
    if (len < 0.001f) return { false, origin, 0 };
    dir /= len;

    sf::Vector2f pos = origin;
    float traveled   = 0.0f;
    const float step = 2.0f;

    while (traveled < maxDist)
    {
        pos      += dir * step;
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
    float angleStart = facingAngle - FOV / 2.0f;
    sf::Color rayColor = playerVisible ? sf::Color(255, 100, 0, 180)
                                       : sf::Color(255, 255, 0, 120);

    for (int i = 0; i < rayCount; i++)
    {
        float angle = angleStart + (FOV / static_cast<float>(rayCount)) * i;
        sf::Vector2f dir(std::cos(angle * PI / 180.f),
                         std::sin(angle * PI / 180.f));

        RayHit hit = CastRay(grid, position, dir, 300.0f);

        sf::Vertex line[2];
        line[0].position = position * mapScale;
        line[0].color    = rayColor;
        line[1].position = hit.point * mapScale;
        line[1].color    = rayColor;

        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
}

// ================================================================== PATROL SETUP
void AgentBase::SetPatrolPoints()
{
    patrolPoints[0] = position;
    for (int i = 1; i < PATROL_COUNT; i++)
    {
        sf::Vector2f prev = patrolPoints[i - 1];
        if (prev.x < 725.0f && prev.y < 545.0f)
            patrolPoints[i] = { prev.x + 45.0f, prev.y + 25.0f };
        else
            patrolPoints[i] = { prev.x - 325.0f, prev.y - 270.0f };
    }
}