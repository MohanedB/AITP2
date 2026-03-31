#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <cmath>
#include "Character/Intrus.h"
#include "Character/Key.h"
#include "Map/Grid.h"
#include "AI/Pathfinder.h"
#include "UI/HUD.h"
#include "UI/EndScreen.h"
#include "Goal/Goal.h"
#include "AgentBase/AgentBase.h"
#include "GOB/Blackboard.h"

static const sf::Vector2f BREAK_ROOM_POS(400.0f, 300.0f);

std::vector<AgentBase> CreerAgents() {
    struct ConfigAgent {
        sf::Vector2f spawn;
        std::vector<sf::Vector2f> patrouille;
    };

    std::vector<ConfigAgent> configs = {
        // Agent 1 - Zone haut-gauche
        {{ 30,  190}, {{30,30},   {170,30},  {170,250}, {30,250}}},
        // Agent 2 - Zone haut-centre
        {{320,  30}, {{320,30},  {480,30},  {480,150}, {320,150}}},
        // Agent 3 - Zone haut-droite
        {{580,  30}, {{580,30},  {760,30},  {760,200}, {580,200}}},
        // Agent 4 - Zone milieu-gauche
        {{150, 350}, {{150,340}, {170,340}, {150,530}, {30,530}}},
        // Agent 5 - Zone centre-bas
        {{330, 450}, {{330,450}, {490,450}, {490,570}, {330,570}}},
        // Agent 6 - Zone milieu-droite
        {{610, 220}, {{610,220}, {760,220}, {760,400}, {650,410}}},
        // Agent 7 - Zone bas-gauche
        {{ 30, 500}, {{30,560},  {150,560}, {150,450}, {30,450}}},
        // Agent 8 - Zone bas-centre
        {{280, 500}, {{280,570}, {460,570}, {460,450}, {280,450}}},
        // Agent 9 - Zone bas-droite
        {{600, 500}, {{600,560}, {760,560}, {760,440}, {600,440}}},
        // Agent 10 - Patrouille large centre
        {{390, 310}, {{390,280}, {540,280}, {540,420}, {370,420}}},
    };

    std::vector<AgentBase> agents;
    for (int i = 0; i < (int)configs.size(); i++) {
        agents.emplace_back(i, configs[i].spawn, BREAK_ROOM_POS, configs[i].patrouille);
    }
    return agents;
}

void ResetGame(Intrus& joueur, std::vector<AgentBase>& ennemis,
               Key& cle, Blackboard& bb, bool& gameOver, bool& joueurALaCle)
{
    joueur       = Intrus(sf::Vector2f(30.0f, 30.0f));
    ennemis      = CreerAgents();
    cle          = Key(sf::Vector2f(390.0f, 550.0f));
    bb.EffacerAlerte();
    gameOver     = false;
    joueurALaCle = false;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({1000, 600}), "PGJ1403 - TP3 GOB Multi-Agents");
    window.setFramerateLimit(60);

    // Créer le monde
    Grid  gameWorld(40, 30, 20.0f);
    Goal  sortie(sf::Vector2f(760.0f, 560.0f));
    Key   cle(sf::Vector2f(390.0f, 550.0f));
    
    Intrus               joueur(sf::Vector2f(30.0f, 30.0f));
    std::vector<AgentBase> ennemis = CreerAgents();
    
    HUD       hud;
    EndScreen ecranFin(window.getSize());
    
    Blackboard bb;

    sf::Clock clock;
    bool gameOver     = false;
    bool joueurALaCle = false;
    bool procheSortieSansCle = false;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f; // Éviter les gros sauts de temps
        
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();

                // Rejouer avec R quand la partie est terminée
                if (key->code == sf::Keyboard::Key::R && gameOver)
                    ResetGame(joueur, ennemis, cle, bb, gameOver, joueurALaCle);
            }

            // Clic sur le bouton "Rejouer"
            if (gameOver && ecranFin.HandleEvent(*event, window))
                ResetGame(joueur, ennemis, cle, bb, gameOver, joueurALaCle);
        }
        
        if (!gameOver)
        {
            bb.Update(dt);
            
            joueur.Update(dt, gameWorld);
            sf::Vector2f posJoueur = joueur.GetPosition();
            
            bool cletaitRamassee = cle.IsPickedUp();
            cle.Update(posJoueur);
            if (!cletaitRamassee && cle.IsPickedUp())
                joueurALaCle = true;
            
            std::vector<AgentBase*> pointeursAgents;
            for (auto& a : ennemis)
                pointeursAgents.push_back(&a);
            
            for (auto& ennemi : ennemis)
            {
                ennemi.SetPlayerPosition(posJoueur);
                ennemi.Update(dt, gameWorld, bb, pointeursAgents);
                
                if (ennemi.ACaptureJoueur())
                {
                    ecranFin.Show(EndResult::Captured);
                    gameOver = true;
                    break;
                }
            }
            
            if (!gameOver)
            {
                sf::Vector2f posSortie = sortie.GetPosition();
                float dx = posJoueur.x - posSortie.x;
                float dy = posJoueur.y - posSortie.y;
                float distSortie = std::sqrt(dx * dx + dy * dy);

                procheSortieSansCle = false;

                if (distSortie < 30.0f)
                {
                    if (joueurALaCle)
                    {
                        ecranFin.Show(EndResult::Escaped);
                        gameOver = true;
                    }
                    else
                    {
                        procheSortieSansCle = true;
                    }
                }
            }
            
            std::vector<std::string> etatsAgents;
            for (auto& a : ennemis)
                etatsAgents.push_back(a.GetGoalString());

            hud.Update(dt, etatsAgents, joueurALaCle, procheSortieSansCle, bb.alerteActive);
        }
        
        window.clear(sf::Color(12, 12, 18));
        
        gameWorld.Draw(window);
        
        sortie.Draw(window);
        cle.Draw(window);
        
        joueur.Draw(window);
        
        for (auto& ennemi : ennemis)
        {
            ennemi.DrawRayCast(window, gameWorld);
            ennemi.Draw(window);
        }
        
        hud.Draw(window);
        
        if (gameOver)
            ecranFin.Draw(window);

        window.display();
    }

    return 0;
}