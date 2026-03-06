#include "AgentBase.h"

AgentBase::AgentBase(sf::Vector2f startPos)
{
    radius = 8.0f;
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin({ radius, radius });
    position = startPos;
    shape.setPosition(position);
    
    speed = 50.0f; 
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
    sf::Vector2f target = patrolPoints[currentPatrolPoint];
    
    sf::Vector2f movement = target - position;
    float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
    
    if (length < 10.0f)
    {
        currentPatrolPoint = (currentPatrolPoint + 1) % 11;
        return;
    }
    
    movement /= length;
    
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

    bool collided = false;
    
    if (!isWall(nextPosX.x - radius, position.y - radius + 1) &&
        !isWall(nextPosX.x + radius, position.y - radius + 1) &&
        !isWall(nextPosX.x - radius, position.y + radius - 1) &&
        !isWall(nextPosX.x + radius, position.y + radius - 1)) {
        position.x = nextPosX.x;
    }
    else {
        collided = true;
    }
    
    if (!isWall(position.x - radius + 1, nextPosY.y - radius) &&
        !isWall(position.x + radius - 1, nextPosY.y - radius) &&
        !isWall(position.x - radius + 1, nextPosY.y + radius) &&
        !isWall(position.x + radius - 1, nextPosY.y + radius)) {
        position.y = nextPosY.y;
    }
    else {
        collided = true;
    }

    
    if (collided)
    {
        position -= movement * speed * deltaTime * 2.0f;
        
        sf::Vector2f right(movement.y, -movement.x);
        
        float rlen = std::sqrt(right.x * right.x + right.y * right.y);
        if (rlen != 0)
            right /= rlen;
        
        position += right * speed * deltaTime * 1.5f;
        
        shape.setPosition(position);
        
        facingAngle = std::atan2(movement.y, movement.x) * 180.f / 3.14159265f;

        return;
    }

    
    shape.setPosition(position);
    
    facingAngle = std::atan2(movement.y, movement.x) * 180.f / 3.14159265f;
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
    float step = 2.0f;

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
        if(patrolPoints[i - 1].x < 725.0f && patrolPoints[i - 1].y < 545.0f)
        {
            patrolPoints[i] = sf::Vector2f(patrolPoints[i - 1].x + 45.0f, patrolPoints[i - 1].y + 25.0f);
        }
        else
        {
            patrolPoints[i] = sf::Vector2f(patrolPoints[i - 1].x - 325.0f, patrolPoints[i - 1].y - 270.0f);
        }
    }
}