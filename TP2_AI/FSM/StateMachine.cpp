#include "StateMachine.h"

StateMachine::~StateMachine()
{
    currentState = States::Patrouille;
}

std::string StateMachine::GetCurrentState()
{
    switch (currentState)
    {
        case States::Patrouille:
        return "État FSM: Patrouille";

        case States::Poursuite:
        return "État FSM: Poursuite";

        case States::Retour:
        return "État FSM: Retour";
        
        default:
        return "Erreur d'état";
    }
}

