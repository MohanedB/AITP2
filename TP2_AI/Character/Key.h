#pragma once
#include <SFML/Graphics.hpp>

class Key {
public:
    Key(sf::Vector2f pos);

    void         Update(sf::Vector2f playerPos);
    void         Draw(sf::RenderWindow& window) const;

    bool         IsPickedUp()  const;
    sf::Vector2f GetPosition() const;

private:
    sf::CircleShape shape;
    sf::Vector2f    position;
    bool            pickedUp;
    float           pickupRadius;
};