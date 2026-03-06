#include "AgentBase.h"

AgentBase::AgentBase(sf::Vector2f startPos)
{
    radius = 8.0f;
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin({ radius, radius });
    position = startPos;
    shape.setPosition(position);
    
    speed = 200.0f; 
}

void AgentBase::Draw(sf::RenderWindow& window)
{
    window.draw(shape);
}

void AgentBase::SetPlayerPosition(sf::Vector2f position)
{
    playerPosition = position;
}

sf::Vector2f AgentBase::GetPlayerPosition()
{
    return playerPosition;
}

sf::Vector2f AgentBase::GetPosition()
{
    return position;
}


void AgentBase::Update(float deltaTime, Grid& grid)
{
     sf::Vector2f movement(0.0f, 0.0f);
    
    if (movement.x != 0.0f || movement.y != 0.0f) {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement.x /= length;
        movement.y /= length;
    }
    
    sf::Vector2f nextPosX = position;
    nextPosX.x += movement.x * speed * deltaTime;

    sf::Vector2f nextPosY = position;
    nextPosY.y += movement.y * speed * deltaTime;
    
    auto isWall = [&](float px, float py) {
        float tileSize = grid.getTileSize();
        int gridX = static_cast<int>(px / tileSize);
        int gridY = static_cast<int>(py / tileSize);
        Node* node = grid.getNode(gridX, gridY);
        return (node == nullptr || node->isObstacle);
    };
    
    if (!isWall(nextPosX.x - radius, position.y - radius + 1) &&
        !isWall(nextPosX.x + radius, position.y - radius + 1) &&
        !isWall(nextPosX.x - radius, position.y + radius - 1) &&
        !isWall(nextPosX.x + radius, position.y + radius - 1)) {
        position.x = nextPosX.x;
    }
    
    if (!isWall(position.x - radius + 1, nextPosY.y - radius) &&
        !isWall(position.x + radius - 1, nextPosY.y - radius) &&
        !isWall(position.x - radius + 1, nextPosY.y + radius) &&
        !isWall(position.x + radius - 1, nextPosY.y + radius)) {
        position.y = nextPosY.y;
    }
    
    shape.setPosition(position); 
}

StateMachine& AgentBase::GetEnnemyState()
{
    return ennemyState;
}

RayHit AgentBase::CastRay(Grid& grid, sf::Vector2f origin, sf::Vector2f dir, float maxDist)
{
    float tileSize = grid.getTileSize();

    // Normaliser la direction
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len == 0) return { false, origin, 0 };
    dir /= len;

    sf::Vector2f pos = origin;
    float traveled = 0.0f;
    float step = 2.0f; // précision du raycast

    while (traveled < maxDist)
    {
        pos += dir * step;
        traveled += step;

        int gx = static_cast<int>(pos.x / tileSize);
        int gy = static_cast<int>(pos.y / tileSize);

        Node* node = grid.getNode(gx, gy);

        if (node == nullptr || node->isObstacle)
        {
            return { true, pos, traveled };
        }
    }

    return { false, pos, maxDist };
}


void AgentBase::RayCast(sf::RenderWindow& window, Grid& grid, float mapScale)
{
    float angleStart = facingAngle - FOV / 2.0f;

    for (int i = 0; i < rayCount; i++)
    {
        float angle = angleStart + (FOV / rayCount) * i;

        sf::Vector2f dir(std::cos(angle * 3.14159f / 180),
                         std::sin(angle * 3.14159f / 180));

        RayHit hit = CastRay(grid, position, dir, 300.0f);

        // Dessin mini-map
        sf::Vertex line[2];
        line[0] = sf::Vertex();
        line[0].color = sf::Color::Yellow;
        line[0].position = position * mapScale;
        line[1].color = sf::Color::Yellow;
        line[1].position = hit.point * mapScale;

        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
}

void AgentBase::SetPatrolPoints()
{
    patrolPoints[0] = position;
    for (int i = 1; i < 11; i++)
    {
        if(patrolPoints[i - 1].x < 770.0f && patrolPoints[i - 1].y < 570.0f)
        {
            patrolPoints[i] = sf::Vector2f(patrolPoints[i - 1].x + 30.0f, patrolPoints[i - 1].y + 30.0f);
        }
        else
        {
            patrolPoints[i] = sf::Vector2f(patrolPoints[i - 1].x - 150.0f, patrolPoints[i - 1].y - 150.0f);
        }
    }
}
