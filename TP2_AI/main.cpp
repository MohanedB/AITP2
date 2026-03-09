#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include "Character/Intrus.h"
#include "Map/Grid.h"
#include "AI/Pathfinder.h"
#include "UI/HUD.h"
#include "UI/EndScreen.h"
#include "Goal/Goal.h"
#include "AgentBase/AgentBase.h"
#include "FSM/StateMachine.h"

void ResetGame(Intrus& joueur, std::vector<AgentBase>& ennemis, bool& gameOver)
{
    joueur = Intrus(sf::Vector2f(30.0f, 30.0f));

    ennemis.clear();
    ennemis.push_back(AgentBase(sf::Vector2f(770.0f, 570.0f)));
    ennemis.push_back(AgentBase(sf::Vector2f(400.0f, 300.0f)));
    ennemis.push_back(AgentBase(sf::Vector2f(100.0f, 500.0f)));

    for (int i = 0; i < (int)ennemis.size(); i++)
        ennemis[i].SetPatrolPoints();

    gameOver = false;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1000, 600 }), "PGJ1403 - TP2 Final");
    window.setFramerateLimit(60);

    Grid gameWorld(40, 30, 20.0f);
    Intrus joueur(sf::Vector2f(30.0f, 30.0f));
    Goal objectif(sf::Vector2f(760.0f, 560.0f)); // bas droite
    HUD interfaceJoueur;
    EndScreen ecranFin(window.getSize());

    std::vector<AgentBase> ennemis;
    ennemis.push_back(AgentBase(sf::Vector2f(770.0f, 570.0f)));
    ennemis.push_back(AgentBase(sf::Vector2f(400.0f, 300.0f)));
    ennemis.push_back(AgentBase(sf::Vector2f(100.0f, 500.0f)));
    for (int i = 0; i < (int)ennemis.size(); i++)
        ennemis[i].SetPatrolPoints();

    sf::Clock clock;
    bool showDebugPath = true;
    bool gameOver = false;

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        // --- EVENTS ---
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();

                if (key->code == sf::Keyboard::Key::T)
                    showDebugPath = !showDebugPath;

                if (key->code == sf::Keyboard::Key::R && gameOver)
                    ResetGame(joueur, ennemis, gameOver);
            }

            if (gameOver && ecranFin.HandleEvent(*event, window))
                ResetGame(joueur, ennemis, gameOver);
        }

        // --- UPDATE ---
        if (!gameOver)
        {
            joueur.Update(deltaTime, gameWorld);

            for (int i = 0; i < (int)ennemis.size(); i++)
            {
                ennemis[i].SetPlayerPosition(joueur.GetPosition());
                ennemis[i].Update(deltaTime, gameWorld);

                if (ennemis[i].HasCapturedPlayer())
                {
                    ecranFin.Show(EndResult::Captured);
                    gameOver = true;
                }
            }

            // Verifier si le joueur a atteint l'objectif
            sf::Vector2f joueurPos = joueur.GetPosition();
            sf::Vector2f goalPos   = objectif.GetPosition();
            float dx = joueurPos.x - goalPos.x;
            float dy = joueurPos.y - goalPos.y;
            float distToGoal = std::sqrt(dx * dx + dy * dy);

            if (distToGoal < 30.0f)
            {
                ecranFin.Show(EndResult::Escaped);
                gameOver = true;
            }

            // Passer l'etat FSM de chaque ennemi au HUD
            std::vector<StateMachine*> states;
            for (int i = 0; i < (int)ennemis.size(); i++)
                states.push_back(&ennemis[i].GetEnnemyStateMachine());
            interfaceJoueur.Update(deltaTime, states);
        }

        // --- PATHFINDING DEBUG ---
        std::vector<sf::Vector2f> debugPath;
        if (showDebugPath && !gameOver)
            debugPath = Pathfinder::FindPath(gameWorld, ennemis[0].GetPosition(), joueur.GetPosition());

        // --- RENDER ---
        window.clear(sf::Color::Black);

        gameWorld.Draw(window);

        if (showDebugPath && debugPath.size() > 1)
        {
            sf::VertexArray lines(sf::PrimitiveType::LineStrip, debugPath.size());
            for (int i = 0; i < (int)debugPath.size(); i++)
            {
                lines[i].position = debugPath[i];
                lines[i].color    = sf::Color::Red;
            }
            window.draw(lines);
        }

        objectif.Draw(window);
        joueur.Draw(window);

        for (int i = 0; i < (int)ennemis.size(); i++)
        {
            ennemis[i].Draw(window);
            ennemis[i].RayCast(window, gameWorld, 1.0f);
        }

        interfaceJoueur.Draw(window);

        if (gameOver)
            ecranFin.Draw(window);

        window.display();
    }

    return 0;
}