#pragma once
#include <string>

enum class GOBGoal {
    Patrouiller,
    Poursuivre,
    RepondreAlerte,
    AllerEnPause,
    JaserCollegue
};

inline std::string GoalToString(GOBGoal g) {
    if (g == GOBGoal::Patrouiller)    return "Patrouille";
    if (g == GOBGoal::Poursuivre)     return "Poursuite";
    if (g == GOBGoal::RepondreAlerte) return "Alerte";
    if (g == GOBGoal::AllerEnPause)   return "Pause";
    if (g == GOBGoal::JaserCollegue)  return "Jase";
    return "Inconnu";
}