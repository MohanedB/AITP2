#include "StateMachine.h"

StateMachine::StateMachine() : currentState(States::Patrouille) {}

void StateMachine::SetState(States newState)   { currentState = newState; }
States StateMachine::GetState() const          { return currentState; }

std::string StateMachine::GetCurrentState() const
{
    switch (currentState)
    {
    case States::Patrouille: return "État FSM: Patrouille";
    case States::Poursuite:  return "État FSM: Poursuite";
    case States::Retour:     return "État FSM: Retour";
    default:                 return "Erreur d'état";
    }
}