#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include "Character/Intrus.h"
#include "Map/Grid.h"
#include "AI/Pathfinder.h"
#include "UI/HUD.h"

int main() {
    // Fenêtre 1000x600 : 800 pour la map + 200 pour la barre latérale (HUD)
    sf::RenderWindow window(sf::VideoMode({ 1000, 600 }), "PGJ1403 - Infrastructure Finale - Personne 1");
    window.setFramerateLimit(60);

    Grid gameWorld(40, 30, 20.0f);
    Intrus joueur({ 30.0f, 30.0f });
    HUD interfaceJoueur;
    
    sf::Clock clock;
    bool showDebugPath = true; // Pour activer/desactiver le test du A*

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
                // Appuie sur 'T' pour toggler le chemin de test
                if (keyPressed->code == sf::Keyboard::Key::T) {
                    showDebugPath = !showDebugPath;
                }
            }
        }

        // --- 1. UPDATE ---
        joueur.Update(deltaTime, gameWorld);
        
        // On simule l'état pour le HUD
        interfaceJoueur.Update(deltaTime, showDebugPath ? "DEBUG PATH" : "MANUAL CONTROL");

        // --- 2. PATHFINDING TEST ---
        std::vector<sf::Vector2f> currentPath;
        if (showDebugPath) {
            sf::Vector2i mousePosi = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosf(static_cast<float>(mousePosi.x), static_cast<float>(mousePosi.y));
            
            // Calcul du A* du joueur vers la souris
            currentPath = Pathfinder::FindPath(gameWorld, joueur.GetPosition(), mousePosf);
        }

        // --- 3. RENDER ---
        window.clear(sf::Color::Black);
        
        gameWorld.Draw(window); // Dessine le labyrinthe
        
        // Dessin du chemin rouge 
        if (showDebugPath && currentPath.size() > 1) {
            sf::VertexArray lines(sf::PrimitiveType::LineStrip, currentPath.size());
            for (size_t i = 0; i < currentPath.size(); ++i) {
                lines[i].position = currentPath[i];
                lines[i].color = sf::Color::Red;
            }
            window.draw(lines);
        }

        joueur.Draw(window); // Dessine l'intrus WASD
        interfaceJoueur.Draw(window); // Dessine le HUD
        
        window.display();
    }
    return 0;
}