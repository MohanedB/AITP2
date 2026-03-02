#include "Intrus.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

Intrus::Intrus(sf::Vector2f startPos) {
    radius = 8.0f; // Un peu plus petit qu'une tuile (20px) pour passer dans les couloirs
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Green); // L'intrus en vert
    shape.setOrigin({ radius, radius }); // On centre le point d'origine du cercle
    position = startPos;
    shape.setPosition(position);
    
    // Vitesse élevée (en pixels par seconde) car il doit être plus rapide que l'agent
    speed = 200.0f; 
}

void Intrus::Update(float deltaTime, Grid& grid) {
    sf::Vector2f movement(0.0f, 0.0f);

    // Lecture des touches WASD (Avancer, Reculer, Gauche, Droite)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) movement.y -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) movement.y += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) movement.x -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) movement.x += 1.0f;

    // Normalisation pour éviter d'aller plus vite en diagonale
    if (movement.x != 0.0f || movement.y != 0.0f) {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement.x /= length;
        movement.y /= length;
    }

    // On sépare le mouvement X et Y pour permettre de "glisser" sur les murs
    sf::Vector2f nextPosX = position;
    nextPosX.x += movement.x * speed * deltaTime;

    sf::Vector2f nextPosY = position;
    nextPosY.y += movement.y * speed * deltaTime;

    // Fonction lambda rapide pour vérifier si un point précis est dans un mur
    auto isWall = [&](float px, float py) {
        float tileSize = grid.getTileSize();
        int gridX = static_cast<int>(px / tileSize);
        int gridY = static_cast<int>(py / tileSize);
        Node* node = grid.getNode(gridX, gridY);
        return (node == nullptr || node->isObstacle);
    };

    // --- GESTION DES COLLISIONS ---
    // Test de collision sur l'axe X (Gauche / Droite)
    if (!isWall(nextPosX.x - radius, position.y - radius + 1) &&
        !isWall(nextPosX.x + radius, position.y - radius + 1) &&
        !isWall(nextPosX.x - radius, position.y + radius - 1) &&
        !isWall(nextPosX.x + radius, position.y + radius - 1)) {
        position.x = nextPosX.x;
    }

    // Test de collision sur l'axe Y (Haut / Bas)
    if (!isWall(position.x - radius + 1, nextPosY.y - radius) &&
        !isWall(position.x + radius - 1, nextPosY.y - radius) &&
        !isWall(position.x - radius + 1, nextPosY.y + radius) &&
        !isWall(position.x + radius - 1, nextPosY.y + radius)) {
        position.y = nextPosY.y;
    }

    // On applique la position finale validée
    shape.setPosition(position);
}

void Intrus::Draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::Vector2f Intrus::GetPosition() const { 
    return position; 
}