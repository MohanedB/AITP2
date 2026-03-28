#pragma once
#include <SFML/Graphics.hpp>

// Tableau partagé entre tous les agents (comme un walkie talkie)
struct Blackboard {
    bool alerteActive = false;
    sf::Vector2f positionIntrus;
    float timerAlerte = 0.0f;

    void Update(float dt) {
        if (alerteActive) {
            timerAlerte += dt;
            if (timerAlerte > 12.0f) {
                alerteActive = false;
                timerAlerte = 0.0f;
            }
        }
    }

    void SignalerIntrus(sf::Vector2f pos) {
        alerteActive = true;
        positionIntrus = pos;
        timerAlerte = 0.0f;
    }

    void EffacerAlerte() {
        alerteActive = false;
        timerAlerte = 0.0f;
    }
};