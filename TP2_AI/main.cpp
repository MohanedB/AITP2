#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include "Character/Intrus.h"
#include "Map/Grid.h"
#include "AI/Pathfinder.h"
#include "UI/HUD.h"
#include "UI/EndScreen.h"
#include <cmath>
#include "AgentBase/AgentBase.h"
#include "FSM/StateMachine.h"
#include "Goal/Goal.h"

static void ResetGame(Intrus& joueur, std::vector<AgentBase>& ennemis, bool& gameOver)
{
    joueur = Intrus({ 30.0f, 30.0f });
    ennemis.clear();
    ennemis.emplace_back(sf::Vector2f{ 770.0f, 570.0f });
    ennemis.emplace_back(sf::Vector2f{ 400.0f, 300.0f });
    ennemis.emplace_back(sf::Vector2f{ 100.0f, 500.0f });
    for (auto& e : ennemis) e.SetPatrolPoints();
    gameOver = false;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1000, 600 }), "PGJ1403 - TP2 Final");
    window.setFramerateLimit(60);

    Grid gameWorld(40, 30, 20.0f);
    Intrus joueur({ 30.0f, 30.0f });
    Goal objectif({ 400.0f, 30.0f });
    HUD interfaceJoueur;
    EndScreen ecranFin(window.getSize());

    std::vector<AgentBase> ennemis;
    ennemis.emplace_back(sf::Vector2f{ 770.0f, 570.0f });
    ennemis.emplace_back(sf::Vector2f{ 400.0f, 300.0f });
    ennemis.emplace_back(sf::Vector2f{ 100.0f, 500.0f });
    for (auto& e : ennemis) e.SetPatrolPoints();

    sf::Clock clock;
    bool showDebugPath = true;
    bool gameOver      = false;

    auto distSq = [](sf::Vector2f a, sf::Vector2f b) {
        float dx = a.x - b.x, dy = a.y - b.y;
        return dx * dx + dy * dy;
    };

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                    window.close();
                if (keyPressed->code == sf::Keyboard::Key::T)
                    showDebugPath = !showDebugPath;
                if (keyPressed->code == sf::Keyboard::Key::R && gameOver)
                    ResetGame(joueur, ennemis, gameOver);
            }

            if (gameOver && ecranFin.HandleEvent(*event, window))
                ResetGame(joueur, ennemis, gameOver);
        }

        // --- UPDATE ---
        if (!gameOver) {
            joueur.Update(deltaTime, gameWorld);

            for (auto& e : ennemis) {
                e.SetPlayerPosition(joueur.GetPosition());
                e.Update(deltaTime, gameWorld);
                if (e.HasCapturedPlayer()) {
                    ecranFin.Show(EndResult::Captured);
                    gameOver = true;
                }
            }

            if (distSq(joueur.GetPosition(), objectif.GetPosition()) < 30.0f * 30.0f) {
                ecranFin.Show(EndResult::Escaped);
                gameOver = true;
            }

            // HUD affiche l'etat FSM du premier ennemi
            interfaceJoueur.Update(deltaTime, ennemis[0].GetEnnemyState());
        }

        // --- PATHFINDING DEBUG (premier ennemi) ---
        std::vector<sf::Vector2f> currentPath;
        if (showDebugPath && !gameOver)
            currentPath = Pathfinder::FindPath(gameWorld, ennemis[0].GetPosition(), joueur.GetPosition());

        // --- RENDER ---
        window.clear(sf::Color::Black);
        gameWorld.Draw(window);

        if (showDebugPath && currentPath.size() > 1) {
            sf::VertexArray lines(sf::PrimitiveType::LineStrip, currentPath.size());
            for (size_t i = 0; i < currentPath.size(); ++i) {
                lines[i].position = currentPath[i];
                lines[i].color    = sf::Color::Red;
            }
            window.draw(lines);
        }

        objectif.Draw(window);
        joueur.Draw(window);
        for (auto& e : ennemis) {
            e.Draw(window);
            e.RayCast(window, gameWorld, 1.0f);
        }
        interfaceJoueur.Draw(window);

        if (gameOver)
            ecranFin.Draw(window);

        window.display();
    }
    return 0;
}