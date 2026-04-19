#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "../Map/Grid.h"
#include "../AI/Pathfinder.h"
#include "../GOB/GOBGoal.h"
#include "../GOB/Blackboard.h"

struct RayHit {
    bool         hit;
    sf::Vector2f point;
    float        distance;
};

class AgentBase {
public:
    AgentBase(int id, sf::Vector2f startPos, sf::Vector2f pausePos,
              std::vector<sf::Vector2f> pointsPatrouille);

    void Update(float dt, Grid& grid, Blackboard& bb, std::vector<AgentBase*>& agents);
    void Draw(sf::RenderWindow& window);
    void DrawRayCast(sf::RenderWindow& window, Grid& grid);

    // NEW — draws patrol route as dotted loop on the floor
    void DrawPatrolRoute(sf::RenderWindow& window) const;

    void         SetPlayerPosition(sf::Vector2f pos);
    sf::Vector2f GetPosition() const;
    int          GetId()       const;
    bool         ACaptureJoueur() const;
    std::string  GetGoalString()  const;

    RayHit CastRay(Grid& grid, sf::Vector2f origin, sf::Vector2f direction, float distMax);

private:
    void ChoisirGoal(Blackboard& bb, std::vector<AgentBase*>& agents);

    void FairePatrouille(float dt, Grid& grid);
    void FairePoursuive (float dt, Grid& grid);
    void FaireAlerte    (float dt, Grid& grid, std::vector<AgentBase*>& agents);
    void FairePause     (float dt, Grid& grid, std::vector<AgentBase*>& agents);

    sf::Vector2f Seek  (sf::Vector2f cible);
    sf::Vector2f Arrive(sf::Vector2f cible);
    void AppliquerMouvement(sf::Vector2f vitesse, float dt, Grid& grid);
    bool EstMur(Grid& grid, float px, float py);

    void DemanderChemin(Grid& grid, sf::Vector2f cible);
    void SuivreChemin  (float dt, Grid& grid, bool utiliserArrive = false);

    bool VoitJoueur(Grid& grid);

    int          id;
    sf::Vector2f position;
    sf::Vector2f positionJoueur;
    sf::Vector2f positionPause;

    sf::CircleShape forme;
    float vitesse;
    float rayon;
    float distCapture;
    float rayonArrivee;
    float angleFace;
    float FOV;

    std::vector<sf::Vector2f> pointsPatrouille;
    int indexPatrouille;

    std::vector<sf::Vector2f> chemin;
    int   indexChemin;
    float cooldownChemin;

    GOBGoal goalActif;
    bool    joueurVisible;

    float envieDeBreak;
    float envieDeJaser;
    float timerPause;
    bool  enPause;
};