#include "HUD.h"
#include <cmath>
#include <iostream>

HUD::HUD()
    : titleText(font), legendText(font), keyStatusText(font),
      alertStatusText(font), fpsText(font), instructionText(font),
      flashText(font)
{
    fontLoaded = font.openFromFile("arial.ttf") ||
                 font.openFromFile("C:/Windows/Fonts/arial.ttf") ||
                 font.openFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");

    if (!fontLoaded)
        std::cerr << "ERREUR : Police arial.ttf manquante !\n";

    sidebarBg.setSize(sf::Vector2f(200.f, 600.f));
    sidebarBg.setPosition(sf::Vector2f(800.f, 0.f));
    sidebarBg.setFillColor(sf::Color(25, 25, 35));
    sidebarBg.setOutlineThickness(-1.f);
    sidebarBg.setOutlineColor(sf::Color(70, 70, 70));

    titleText.setCharacterSize(16);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(sf::Vector2f(808.f, 6.f));
    titleText.setString("=== AGENTS GOB ===");

    keyStatusText.setCharacterSize(14);
    keyStatusText.setPosition(sf::Vector2f(808.f, 318.f));

    alertStatusText.setCharacterSize(14);
    alertStatusText.setPosition(sf::Vector2f(808.f, 345.f));

    fpsText.setCharacterSize(13);
    fpsText.setFillColor(sf::Color::Yellow);
    fpsText.setPosition(sf::Vector2f(808.f, 372.f));

    legendText.setCharacterSize(11);
    legendText.setFillColor(sf::Color(160, 160, 160));
    legendText.setPosition(sf::Vector2f(808.f, 400.f));
    legendText.setString(
        "Rouge fonce : Surveille\n"
        "Rouge vif   : Poursuite\n"
        "Orange      : Intercepte\n"
        "Jaune       : Alerte\n"
        "Cyan        : En pause\n"
        "Vert        : Jase"
    );

    instructionText.setCharacterSize(12);
    instructionText.setFillColor(sf::Color(130, 130, 130));
    instructionText.setPosition(sf::Vector2f(808.f, 555.f));
    instructionText.setString("WASD:Bouger  ESC:Quitter");

    flashText.setCharacterSize(28);
    flashText.setStyle(sf::Text::Bold);
    flashText.setPosition(sf::Vector2f(400.f, 290.f));
}

sf::Color HUD::ColorForGoal(const std::string& goal) {
    if (goal == "Poursuite")       return sf::Color(255,  50,  50);
    if (goal == "Intercepter")     return sf::Color(255, 160,   0);
    if (goal == "Repond Alerte")   return sf::Color(255, 220,  50);
    if (goal == "En Pause")        return sf::Color( 50, 200, 255);
    if (goal == "Rejoint Collegue")return sf::Color(100, 255, 120);
    return sf::Color(200, 200, 200); // Surveiller Zone
}

void HUD::SetFlash(const std::string& msg, sf::Color col, float duration) {
    if (!fontLoaded) return;
    flashText.setString(msg);
    flashText.setFillColor(col);
    sf::FloatRect b = flashText.getLocalBounds();
    flashText.setOrigin({ b.size.x / 2.f, b.size.y / 2.f });
    flashText.setPosition({ 400.f, 290.f });
    flashTimer = duration;
}

void HUD::Update(float dt,
                 const std::vector<std::string>& agentGoals,
                 bool playerHasKey, bool nearExitNoKey, bool alertActive)
{
    if (!fontLoaded) return;
    
    if (agentTexts.size() != agentGoals.size()) {
        agentTexts.clear();
        for (int i = 0; i < (int)agentGoals.size(); i++) {
            sf::Text t(font);
            t.setCharacterSize(13);
            t.setPosition(sf::Vector2f(808.f, 28.f + i * 27.f));
            agentTexts.push_back(std::move(t));
        }
    }

    for (int i = 0; i < (int)agentGoals.size(); i++) {
        agentTexts[i].setString("Ag" + std::to_string(i+1) + ": " + agentGoals[i]);
        agentTexts[i].setFillColor(ColorForGoal(agentGoals[i]));
    }
    
    if (nearExitNoKey)
        keyStatusText.setString("CLE: Sortie bloquee!"),
        keyStatusText.setFillColor(sf::Color(255, 80, 80));
    else if (playerHasKey)
        keyStatusText.setString("CLE: En possession  "),
        keyStatusText.setFillColor(sf::Color(255, 215, 0));
    else
        keyStatusText.setString("CLE: Non trouvee    "),
        keyStatusText.setFillColor(sf::Color(160, 160, 160));
    
    if (alertActive)
        alertStatusText.setString("BB: ALERTE ACTIVE"),
        alertStatusText.setFillColor(sf::Color(255, 80, 80));
    else
        alertStatusText.setString("BB: Calme          "),
        alertStatusText.setFillColor(sf::Color(80, 200, 80));
    
    if (dt > 0.0f)
        fpsText.setString("FPS: " + std::to_string(static_cast<int>(1.0f / dt)));
    
    if (playerHasKey && !prevHasKey)
        SetFlash("Cle obtenue ! Rejoignez la sortie !", sf::Color(255, 215, 0), 3.0f);

    if (nearExitNoKey && !prevNearExit)
        SetFlash("Trouvez la cle d'abord !", sf::Color(255, 80, 80), 2.0f);

    prevHasKey   = playerHasKey;
    prevNearExit = nearExitNoKey;

    if (flashTimer > 0.0f) flashTimer -= dt;
}

void HUD::Draw(sf::RenderWindow& window) {
    window.draw(sidebarBg);
    if (!fontLoaded) return;

    window.draw(titleText);
    for (auto& t : agentTexts) window.draw(t);
    window.draw(keyStatusText);
    window.draw(alertStatusText);
    window.draw(fpsText);
    window.draw(legendText);
    window.draw(instructionText);

    if (flashTimer > 0.0f) {
        // Fond semi-transparent pour le flash
        sf::RectangleShape bg(sf::Vector2f(660.f, 50.f));
        bg.setFillColor(sf::Color(0, 0, 0, 180));
        bg.setOrigin({ 330.f, 25.f });
        bg.setPosition({ 400.f, 290.f });
        window.draw(bg);
        window.draw(flashText);
    }
}