#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <cmath>
#include "Character/Intrus.h"
#include "Character/Key.h"
#include "Map/Grid.h"
#include "Map/LevelGenerator.h"
#include "AI/Pathfinder.h"
#include "UI/HUD.h"
#include "UI/EndScreen.h"
#include "Goal/Goal.h"
#include "AgentBase/AgentBase.h"
#include "GOB/Blackboard.h"

// ─────────────────────────────────────────────────────────────────────────────

std::vector<AgentBase> CreerAgents(const LevelData& data) {
    std::vector<AgentBase> agents;
    int n = (int)data.agentSpawns.size();
    for (int i = 0; i < n; i++)
        agents.emplace_back(i, data.agentSpawns[i], data.breakRoomPos, data.patrolRoutes[i]);
    return agents;
}

void ResetGame(Grid& grid, LevelData& data,
               Intrus& joueur, std::vector<AgentBase>& ennemis,
               Key& cle, Goal& sortie,
               Blackboard& bb, bool& gameOver, bool& joueurALaCle)
{
    data    = LevelGenerator::Generate(grid);
    joueur  = Intrus(data.playerSpawn);
    ennemis = CreerAgents(data);
    cle     = Key(data.keyPos);
    sortie  = Goal(data.exitPos);
    bb.EffacerAlerte();
    gameOver     = false;
    joueurALaCle = false;
}

// ─────────────────────────────────────────────────────────────────────────────

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1000, 600 }),
                            "PGJ1403 - TP4 Generation Procedurale");
    window.setFramerateLimit(60);

    Grid grid(40, 30, 20.0f);

    LevelData data = LevelGenerator::Generate(grid);

    Goal   sortie(data.exitPos);
    Key    cle(data.keyPos);
    Intrus joueur(data.playerSpawn);
    std::vector<AgentBase> ennemis = CreerAgents(data);

    HUD        hud;
    EndScreen  ecranFin(window.getSize());
    Blackboard bb;

    sf::Clock clock;
    bool gameOver            = false;
    bool joueurALaCle        = false;
    bool procheSortieSansCle = false;

    // Press H to show/hide the patrol route dots on the map
    bool showPatrolRoutes = true;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        // ── Events ──────────────────────────────────────────────────────────
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();

                // H — toggle patrol route visibility
                if (key->code == sf::Keyboard::Key::H)
                    showPatrolRoutes = !showPatrolRoutes;

                // R — restart with a brand-new map
                if (key->code == sf::Keyboard::Key::R && gameOver)
                    ResetGame(grid, data, joueur, ennemis, cle, sortie,
                              bb, gameOver, joueurALaCle);
            }

            if (gameOver && ecranFin.HandleEvent(*event, window))
                ResetGame(grid, data, joueur, ennemis, cle, sortie,
                          bb, gameOver, joueurALaCle);
        }

        // ── Update ──────────────────────────────────────────────────────────
        if (!gameOver) {
            bb.Update(dt);

            joueur.Update(dt, grid);
            sf::Vector2f posJoueur = joueur.GetPosition();

            bool cleTaitRamassee = cle.IsPickedUp();
            cle.Update(posJoueur);
            if (!cleTaitRamassee && cle.IsPickedUp())
                joueurALaCle = true;

            std::vector<AgentBase*> pointeursAgents;
            for (auto& a : ennemis)
                pointeursAgents.push_back(&a);

            for (auto& ennemi : ennemis) {
                ennemi.SetPlayerPosition(posJoueur);
                ennemi.Update(dt, grid, bb, pointeursAgents);

                if (ennemi.ACaptureJoueur()) {
                    ecranFin.Show(EndResult::Captured);
                    gameOver = true;
                    break;
                }
            }

            if (!gameOver) {
                sf::Vector2f posSortie = sortie.GetPosition();
                float dx = posJoueur.x - posSortie.x;
                float dy = posJoueur.y - posSortie.y;
                float distSortie = std::sqrt(dx * dx + dy * dy);

                procheSortieSansCle = false;
                if (distSortie < 30.0f) {
                    if (joueurALaCle) {
                        ecranFin.Show(EndResult::Escaped);
                        gameOver = true;
                    } else {
                        procheSortieSansCle = true;
                    }
                }
            }

            std::vector<std::string> etatsAgents;
            for (auto& a : ennemis)
                etatsAgents.push_back(a.GetGoalString());

            hud.Update(dt, etatsAgents, joueurALaCle, procheSortieSansCle, bb.alerteActive);
        }

        // ── Draw ────────────────────────────────────────────────────────────
        window.clear(sf::Color(12, 12, 18));

        grid.Draw(window);

        // Patrol routes are drawn under everything else.
        // Press H to hide them.
        if (showPatrolRoutes) {
            for (auto& ennemi : ennemis)
                ennemi.DrawPatrolRoute(window);
        }

        sortie.Draw(window);
        cle.Draw(window);
        joueur.Draw(window);

        for (auto& ennemi : ennemis) {
            ennemi.DrawRayCast(window, grid);
            ennemi.Draw(window);
        }

        hud.Draw(window);

        if (gameOver)
            ecranFin.Draw(window);

        window.display();
    }

    return 0;
}