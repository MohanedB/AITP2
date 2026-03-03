#pragma once
#include <string>

enum States
{
    Patrouille,
    Poursuite,
    Retour
};

class StateMachine
{
public:
    ~StateMachine();
    std::string GetCurrentState();
    
private:
States currentState;
    
};
