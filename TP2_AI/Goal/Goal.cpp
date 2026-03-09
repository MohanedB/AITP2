#include "Goal.h"

Goal::Goal(sf::Vector2f startPos)
{
    radius = 20.0f;
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Blue); // Le but en bleu
    shape.setOrigin({ radius, radius });
    position = startPos;
    shape.setPosition(position);
}


void Goal::Draw(sf::RenderWindow& window) const
{
    window.draw(shape);
}

sf::Vector2f Goal::GetPosition() const
{
    return position;
}

