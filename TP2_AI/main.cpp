#include <SFML/Graphics.hpp>
#include <optional>
#include "Character/Intrus.h"
#include "Map/Grid.h"
#include "UI/HUD.h"

int main() {
    // ON AGRANDIT LA FENÊTRE : 800 (Map) + 200 (HUD) = 1000 pixels de large
    sf::RenderWindow window(sf::VideoMode({ 1000, 600 }), "PGJ1403 - TP2 : Setup HUD Propre");
    window.setFramerateLimit(60);

    Grid gameWorld(40, 30, 20.0f);
    Intrus joueur({ 30.0f, 30.0f });
    HUD interfaceJoueur;
    
    sf::Clock clock;

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
            }
        }

        // --- UPDATE ---
        joueur.Update(deltaTime, gameWorld);
        interfaceJoueur.Update(deltaTime, "PATROUILLE");

        // --- RENDER ---
        window.clear(sf::Color::Black);
        
        // La zone de jeu s'affiche de 0 a 800
        gameWorld.Draw(window);
        joueur.Draw(window);
        
        // Le HUD s'affiche dans sa barre a droite de 800 a 1000
        interfaceJoueur.Draw(window);
        
        window.display();
    }
    
    return 0;
}