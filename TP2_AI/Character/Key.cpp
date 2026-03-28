#include "Key.h"
#include <cmath>

Key::Key(sf::Vector2f pos)
    : position(pos), pickedUp(false), pickupRadius(16.0f)
{
    shape.setRadius(10.0f);
    shape.setFillColor(sf::Color(255, 215, 0));  
    shape.setOutlineThickness(2.5f);
    shape.setOutlineColor(sf::Color(180, 130, 0));
    shape.setOrigin({ 10.0f, 10.0f });
    shape.setPosition(position);
}

void Key::Update(sf::Vector2f playerPos) {
    if (pickedUp) return;
    float dx = playerPos.x - position.x;
    float dy = playerPos.y - position.y;
    if (std::sqrt(dx * dx + dy * dy) < pickupRadius)
        pickedUp = true;
}

void Key::Draw(sf::RenderWindow& window) const {
    if (!pickedUp)
        window.draw(shape);
}

bool Key::IsPickedUp()  const { return pickedUp; }
sf::Vector2f Key::GetPosition() const { return position; }