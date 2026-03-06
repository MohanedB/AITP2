#include "HUD.h"
#include <iostream>
#include <cmath>

HUD::HUD() 
    : titleText(font), 
      fsmStateText(font), 
      fpsText(font), 
      instructionText(font) 
{
    fontLoaded = font.openFromFile("arial.ttf");
    
    if (!fontLoaded) {
        std::cerr << "ERREUR : Police arial.ttf manquante !" << std::endl;
    }

    // Fond de la barre latérale (Sidebar)
    sidebarBg.setSize({ 200.f, 600.f });
    sidebarBg.setPosition({ 800.f, 0.f });
    sidebarBg.setFillColor(sf::Color(45, 45, 55)); // Gris bleuté pour séparer du noir
    sidebarBg.setOutlineThickness(-2.f);
    sidebarBg.setOutlineColor(sf::Color(100, 100, 100));

    // Titre
    titleText.setString("INFOS IA");
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    titleText.setPosition({ 815.f, 20.f });

    // État de la FSM 
    fsmStateText.setCharacterSize(18);
    fsmStateText.setFillColor(sf::Color::White);
    fsmStateText.setPosition({ 815.f, 80.f });

    // FPS
    fpsText.setCharacterSize(16);
    fpsText.setFillColor(sf::Color::Yellow);
    fpsText.setPosition({ 815.f, 130.f });

    // Instructions
    instructionText.setCharacterSize(13);
    instructionText.setFillColor(sf::Color(180, 180, 180));
    instructionText.setPosition({ 810.f, 500.f });
    instructionText.setString("T : Toggle Affichage Pathfinding\nWASD : Bouger\nESC : Quitter");
}

void HUD::Update(float deltaTime, StateMachine& state) {
    if (!fontLoaded) return;

    fsmStateText.setString(state.GetCurrentState());

    if (deltaTime > 0.0f) {
        int fps = static_cast<int>(std::round(1.0f / deltaTime));
        fpsText.setString("Performance :\n" + std::to_string(fps) + " FPS");
    }
}

void HUD::Draw(sf::RenderWindow& window) {
    window.draw(sidebarBg); // On dessine le fond en premier
    
    if (fontLoaded) {
        window.draw(titleText);
        window.draw(fsmStateText);
        window.draw(fpsText);
        window.draw(instructionText);
    }
}