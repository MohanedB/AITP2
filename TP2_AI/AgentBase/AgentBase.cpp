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