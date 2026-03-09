#include "HUD.h"
#include <iostream>
#include <cmath>

HUD::HUD()
    : titleText(font),
      fpsText(font),
      instructionText(font)
{
    fontLoaded = font.openFromFile("arial.ttf");

    if (!fontLoaded)
        std::cerr << "ERREUR : Police arial.ttf manquante !" << std::endl;

    sidebarBg.setSize(sf::Vector2f(200.f, 600.f));
    sidebarBg.setPosition(sf::Vector2f(800.f, 0.f));
    sidebarBg.setFillColor(sf::Color(45, 45, 55));
    sidebarBg.setOutlineThickness(-2.f);
    sidebarBg.setOutlineColor(sf::Color(100, 100, 100));

    titleText.setString("INFOS IA");
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    titleText.setPosition(sf::Vector2f(815.f, 20.f));

    fpsText.setCharacterSize(16);
    fpsText.setFillColor(sf::Color::Yellow);
    fpsText.setPosition(sf::Vector2f(815.f, 350.f));

    instructionText.setCharacterSize(13);
    instructionText.setFillColor(sf::Color(180, 180, 180));
    instructionText.setPosition(sf::Vector2f(810.f, 500.f));
    instructionText.setString("T : Toggle Pathfinding\nWASD : Bouger\nESC : Quitter");
}

void HUD::Update(float deltaTime, std::vector<StateMachine*> states)
{
    if (!fontLoaded)
        return;

    // Creer ou mettre a jour un sf::Text par ennemi
    // Si le nombre d'ennemis a change, reconstruire la liste
    if (enemyStateTexts.size() != states.size())
    {
        enemyStateTexts.clear();
        for (int i = 0; i < (int)states.size(); i++)
        {
            sf::Text t(font);
            t.setCharacterSize(16);
            t.setFillColor(sf::Color::White);
            // Espacer chaque ligne de 60px a partir de y=70
            t.setPosition(sf::Vector2f(815.f, 70.f + i * 85.f));
            enemyStateTexts.push_back(t);
        }
    }

    // Mettre a jour le texte de chaque ennemi
    for (int i = 0; i < (int)states.size(); i++)
    {
        std::string label = "Ennemi " + std::to_string(i + 1) + " :\n";
        std::string stateName = states[i]->GetCurrentState();

        // Garder juste le nom de l'etat (apres "Etat FSM: ")
        std::string prefix = "Etat FSM: ";
        size_t pos = stateName.find(prefix);
        if (pos != std::string::npos)
            stateName = stateName.substr(pos + prefix.size());

        // Couleur selon l'etat
        if (states[i]->GetState() == States::Poursuite)
            enemyStateTexts[i].setFillColor(sf::Color::Red);
        else if (states[i]->GetState() == States::Retour)
            enemyStateTexts[i].setFillColor(sf::Color::Yellow);
        else
            enemyStateTexts[i].setFillColor(sf::Color::White);

        enemyStateTexts[i].setString(label + stateName);
    }

    // FPS
    if (deltaTime > 0.0f)
    {
        int fps = static_cast<int>(std::round(1.0f / deltaTime));
        fpsText.setString("Performance :\n" + std::to_string(fps) + " FPS");
    }
}

void HUD::Draw(sf::RenderWindow& window)
{
    window.draw(sidebarBg);

    if (fontLoaded)
    {
        window.draw(titleText);

        for (int i = 0; i < (int)enemyStateTexts.size(); i++)
            window.draw(enemyStateTexts[i]);

        window.draw(fpsText);
        window.draw(instructionText);
    }
}