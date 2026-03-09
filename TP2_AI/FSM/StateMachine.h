#pragma once
#include <string>

enum class States
{
    Patrouille,
    Poursuite,
    Retour
};

class StateMachine
{
public:
    StateMachine();

    void        SetState(States newState);
    States      GetState() const;
    std::string GetCurrentState() const; // meme interface pour le HUD

private:
    States currentState;
};