#pragma once
#include <SFML/Graphics.hpp>
#include "../Map/Grid.h"

class Intrus {
private:
    sf::CircleShape shape;
    sf::Vector2f position;
    float speed;
    float radius;

public:
    // Constructeur
    Intrus(sf::Vector2f startPos);

    // Méthodes principales
    void Update(float deltaTime, Grid& grid);
    void Draw(sf::RenderWindow& window);

    // Accesseur pour l'IA
    sf::Vector2f GetPosition() const;
};