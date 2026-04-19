#include "AgentBase.h"
#include <cmath>
#include <cstdlib>

// Fonctions utilitaires simples
float Longueur(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float Distance(sf::Vector2f a, sf::Vector2f b) {
    return Longueur(b - a);
}

AgentBase::AgentBase(int id, sf::Vector2f startPos, sf::Vector2f pausePos,
                     std::vector<sf::Vector2f> pointsPatrouille)
{
    this->id              = id;
    this->position        = startPos;
    this->positionPause   = pausePos;
    this->pointsPatrouille = pointsPatrouille;

    vitesse        = 80.0f;
    rayon          = 8.0f;
    distCapture    = 14.0f;
    rayonArrivee   = 55.0f;
    angleFace      = static_cast<float>(std::rand() % 360);
    FOV            = 70.0f;

    indexPatrouille = 0;
    indexChemin     = 0;
    cooldownChemin  = 0.0f;

    goalActif     = GOBGoal::Patrouiller;
    joueurVisible = false;

    // Offset pour que les agents ne se reposent pas tous en même temps
    envieDeBreak = static_cast<float>((id * 13) % 50);
    envieDeJaser = static_cast<float>((id *  7) % 40);
    timerPause   = 0.0f;
    enPause      = false;

    forme.setRadius(rayon);
    forme.setFillColor(sf::Color(200, 50, 50));
    forme.setOrigin({ rayon, rayon });
    forme.setPosition(position);
}

// ─────────────────────────────────────────────
//  Accesseurs
// ─────────────────────────────────────────────
void         AgentBase::SetPlayerPosition(sf::Vector2f pos) { positionJoueur = pos; }
sf::Vector2f AgentBase::GetPosition()    const { return position; }
int          AgentBase::GetId()          const { return id; }
bool         AgentBase::ACaptureJoueur() const { return Distance(position, positionJoueur) < distCapture; }
std::string  AgentBase::GetGoalString()  const { return GoalToString(goalActif); }

// ─────────────────────────────────────────────
//  Mouvement
// ─────────────────────────────────────────────
sf::Vector2f AgentBase::Seek(sf::Vector2f cible) {
    sf::Vector2f direction = cible - position;
    float len = Longueur(direction);
    if (len < 0.001f) return sf::Vector2f(0, 0);
    return (direction / len) * vitesse;
}

sf::Vector2f AgentBase::Arrive(sf::Vector2f cible) {
    sf::Vector2f direction = cible - position;
    float dist = Longueur(direction);
    if (dist < 0.001f) return sf::Vector2f(0, 0);

    float vitesseDesiree = vitesse;
    if (dist < rayonArrivee)
        vitesseDesiree = vitesse * (dist / rayonArrivee);

    return (direction / dist) * vitesseDesiree;
}

bool AgentBase::EstMur(Grid& grid, float px, float py) {
    int gx = static_cast<int>(px / grid.getTileSize());
    int gy = static_cast<int>(py / grid.getTileSize());
    Node* node = grid.getNode(gx, gy);
    return (node == nullptr || node->isObstacle);
}

void AgentBase::AppliquerMouvement(sf::Vector2f vitesseVec, float dt, Grid& grid) {
    if (Longueur(vitesseVec) < 0.001f) return;

    // Mettre à jour l'angle de regard
    const float PI = 3.14159265f;
    angleFace = std::atan2(vitesseVec.y, vitesseVec.x) * 180.f / PI;

    float r = rayon - 1.0f;

    // Mouvement en X
    sf::Vector2f nextX = position;
    nextX.x += vitesseVec.x * dt;
    if (!EstMur(grid, nextX.x - r, position.y - r) &&
        !EstMur(grid, nextX.x + r, position.y - r) &&
        !EstMur(grid, nextX.x - r, position.y + r) &&
        !EstMur(grid, nextX.x + r, position.y + r))
    {
        position.x = nextX.x;
    }

    // Mouvement en Y
    sf::Vector2f nextY = position;
    nextY.y += vitesseVec.y * dt;
    if (!EstMur(grid, position.x - r, nextY.y - r) &&
        !EstMur(grid, position.x + r, nextY.y - r) &&
        !EstMur(grid, position.x - r, nextY.y + r) &&
        !EstMur(grid, position.x + r, nextY.y + r))
    {
        position.y = nextY.y;
    }

    forme.setPosition(position);
}

void AgentBase::DemanderChemin(Grid& grid, sf::Vector2f cible) {
    chemin       = Pathfinder::FindPath(grid, position, cible);
    indexChemin  = 0;
}

void AgentBase::SuivreChemin(float dt, Grid& grid, bool utiliserArrive) {
    // Passer les waypoints déjà atteints
    while (indexChemin < (int)chemin.size() &&
           Distance(position, chemin[indexChemin]) < 6.0f)
    {
        indexChemin++;
    }

    if (chemin.empty() || indexChemin >= (int)chemin.size())
        return;

    sf::Vector2f waypoint = chemin[indexChemin];
    bool dernierWaypoint  = (indexChemin == (int)chemin.size() - 1);

    sf::Vector2f vel;
    if (utiliserArrive && dernierWaypoint)
        vel = Arrive(waypoint);
    else
        vel = Seek(waypoint);

    AppliquerMouvement(vel, dt, grid);
}

// ─────────────────────────────────────────────
//  Vision (raycasting)
// ─────────────────────────────────────────────
bool AgentBase::VoitJoueur(Grid& grid) {
    sf::Vector2f versJoueur = positionJoueur - position;
    float dist = Longueur(versJoueur);

    if (dist < 0.001f || dist > 300.0f)
        return false;

    // Vérifier si le joueur est dans le champ de vision (FOV)
    const float PI = 3.14159265f;
    float angleVersJoueur = std::atan2(versJoueur.y, versJoueur.x) * 180.f / PI;
    float difference = angleVersJoueur - angleFace;

    while (difference >  180.f) difference -= 360.f;
    while (difference < -180.f) difference += 360.f;

    if (std::abs(difference) > FOV / 2.0f)
        return false;

    // Vérifier s'il y a un mur entre l'agent et le joueur
    RayHit resultat = CastRay(grid, position, versJoueur / dist, dist);
    return !resultat.hit;
}

RayHit AgentBase::CastRay(Grid& grid, sf::Vector2f origin, sf::Vector2f direction, float distMax) {
    float len = Longueur(direction);
    if (len < 0.001f) return { false, origin, 0.f };

    direction = direction / len;
    sf::Vector2f pos = origin;
    float parcouru = 0.0f;

    while (parcouru < distMax) {
        pos += direction * 2.0f;
        parcouru += 2.0f;

        int gx = static_cast<int>(pos.x / grid.getTileSize());
        int gy = static_cast<int>(pos.y / grid.getTileSize());
        Node* node = grid.getNode(gx, gy);

        if (node == nullptr || node->isObstacle)
            return { true, pos, parcouru };
    }

    return { false, pos, distMax };
}

// ─────────────────────────────────────────────
//  GOB - Choisir le meilleur goal
// ─────────────────────────────────────────────
void AgentBase::ChoisirGoal(Blackboard& bb, std::vector<AgentBase*>& agents) {
    // Calculer un score pour chaque goal
    // Plus le score est élevé, plus c'est prioritaire

    float scorePoursuivre     = -9999.0f;
    float scoreAlerte         = -9999.0f;
    float scorePatrouiller    = 50.0f;   // toujours disponible
    float scorePause          = -9999.0f;
    float scoreJaser          = -9999.0f;

    // Poursuivre : seulement si on voit le joueur
    if (joueurVisible)
        scorePoursuivre = 100.0f;

    // Répondre à l'alerte : si quelqu'un a signalé l'intrus et on ne le voit pas
    if (bb.alerteActive && !joueurVisible) {
        float distAlerte = Distance(position, bb.positionIntrus);
        scoreAlerte = 80.0f - distAlerte * 0.01f;
    }

    // Aller en pause : seulement si pas d'alerte et on a besoin de repos
    if (!bb.alerteActive && !joueurVisible && envieDeBreak > 60.0f) {
        float distPause = Distance(position, positionPause);
        scorePause = envieDeBreak - distPause * 0.05f;
    }

    // Jaser avec un collègue : seulement si pas d'alerte et on en a envie
    if (!bb.alerteActive && !joueurVisible && envieDeJaser > 65.0f) {
        // Chercher le collègue le plus proche
        float distMin = 9999.0f;
        for (AgentBase* autre : agents) {
            if (autre->id != id) {
                float d = Distance(position, autre->GetPosition());
                if (d < distMin) distMin = d;
            }
        }
        if (distMin < 300.0f)
            scoreJaser = envieDeJaser - distMin * 0.06f;
    }

    // Trouver le meilleur score
    float meilleurScore = scorePatrouiller;
    GOBGoal meilleurGoal = GOBGoal::Patrouiller;

    if (scorePoursuivre > meilleurScore) {
        meilleurScore = scorePoursuivre;
        meilleurGoal  = GOBGoal::Poursuivre;
    }
    if (scoreAlerte > meilleurScore) {
        meilleurScore = scoreAlerte;
        meilleurGoal  = GOBGoal::RepondreAlerte;
    }
    if (scorePause > meilleurScore) {
        meilleurScore = scorePause;
        meilleurGoal  = GOBGoal::AllerEnPause;
    }
    if (scoreJaser > meilleurScore) {
        meilleurScore = scoreJaser;
        meilleurGoal  = GOBGoal::JaserCollegue;
    }

    // Si le goal change, on réinitialise le chemin
    if (meilleurGoal != goalActif) {
        chemin.clear();
        cooldownChemin = 0.0f;
        enPause = false;
        goalActif = meilleurGoal;
    }
}

// ─────────────────────────────────────────────
//  Comportements
// ─────────────────────────────────────────────
void AgentBase::FairePatrouille(float dt, Grid& grid) {
    if (pointsPatrouille.empty()) return;

    // Si on n'a plus de chemin, aller au prochain point
    if (chemin.empty() || indexChemin >= (int)chemin.size()) {
        indexPatrouille = (indexPatrouille + 1) % (int)pointsPatrouille.size();
        DemanderChemin(grid, pointsPatrouille[indexPatrouille]);
    }

    SuivreChemin(dt, grid, false);
}

void AgentBase::FairePoursuive(float dt, Grid& grid) {
    // Recalculer le chemin régulièrement pour suivre le joueur
    if (cooldownChemin <= 0.0f) {
        DemanderChemin(grid, positionJoueur);
        cooldownChemin = 0.4f;
    }
    SuivreChemin(dt, grid, false);
}

void AgentBase::FaireAlerte(float dt, Grid& grid, std::vector<AgentBase*>& agents) {
    // Calcul d'une position d'encerclement simple :
    // on répartit les agents alertés en cercle autour de la dernière position connue
    sf::Vector2f cible = positionJoueur;

    if (!agents.empty()) {
        // Compter combien d'agents répondent à l'alerte
        int nbRepondants = 0;
        int monIndex = 0;
        for (int i = 0; i < (int)agents.size(); i++) {
            GOBGoal g = agents[i]->goalActif;
            if (g == GOBGoal::RepondreAlerte || g == GOBGoal::Poursuivre) {
                if (agents[i]->id == id) monIndex = nbRepondants;
                nbRepondants++;
            }
        }

        if (nbRepondants > 1) {
            const float PI = 3.14159265f;
            float angle = (2.0f * PI * monIndex) / (float)nbRepondants;
            float rayon = 80.0f;
            cible.x += std::cos(angle) * rayon;
            cible.y += std::sin(angle) * rayon;
        }
    }

    if (chemin.empty() || cooldownChemin <= 0.0f) {
        DemanderChemin(grid, cible);
        cooldownChemin = 1.0f;
    }

    SuivreChemin(dt, grid, false);
}

void AgentBase::FairePause(float dt, Grid& grid, std::vector<AgentBase*>& agents) {
    sf::Vector2f destination = positionPause;

    // Si on jase, aller vers le collègue le plus proche
    if (goalActif == GOBGoal::JaserCollegue) {
        AgentBase* plusProche = nullptr;
        float distMin = 9999.0f;
        for (AgentBase* autre : agents) {
            if (autre->id != id) {
                float d = Distance(position, autre->GetPosition());
                if (d < distMin) { distMin = d; plusProche = autre; }
            }
        }
        if (plusProche != nullptr)
            destination = plusProche->GetPosition();
    }

    // Une fois arrivé, on "se repose"
    if (Distance(position, destination) < 25.0f) {
        timerPause += dt;
        if (timerPause >= 5.0f) {
            // Pause terminée, reset
            timerPause   = 0.0f;
            envieDeBreak = 0.0f;
            envieDeJaser = 0.0f;
            enPause      = false;
            chemin.clear();
        }
        return; // Rester sur place
    }

    if (chemin.empty() || indexChemin >= (int)chemin.size())
        DemanderChemin(grid, destination);

    SuivreChemin(dt, grid, true);
}

// ─────────────────────────────────────────────
//  Update principal
// ─────────────────────────────────────────────
void AgentBase::Update(float dt, Grid& grid, Blackboard& bb, std::vector<AgentBase*>& agents) {
    if (cooldownChemin > 0.0f) cooldownChemin -= dt;

    // Accumuler l'envie de faire une pause avec le temps
    envieDeBreak += dt * 3.0f;
    envieDeJaser += dt * 2.0f;
    if (envieDeBreak > 100.0f) envieDeBreak = 100.0f;
    if (envieDeJaser > 100.0f) envieDeJaser = 100.0f;

    // Détecter le joueur
    joueurVisible = VoitJoueur(grid);

    // Signaler l'intrus à tous les agents si on le voit
    if (joueurVisible)
        bb.SignalerIntrus(positionJoueur);

    // Choisir le meilleur goal (GOB)
    ChoisirGoal(bb, agents);

    // Exécuter le comportement du goal actif
    if (goalActif == GOBGoal::Patrouiller)
        FairePatrouille(dt, grid);
    else if (goalActif == GOBGoal::Poursuivre)
        FairePoursuive(dt, grid);
    else if (goalActif == GOBGoal::RepondreAlerte)
        FaireAlerte(dt, grid, agents);
    else if (goalActif == GOBGoal::AllerEnPause || goalActif == GOBGoal::JaserCollegue)
        FairePause(dt, grid, agents);
}

// ─────────────────────────────────────────────
//  Rendu
// ─────────────────────────────────────────────
void AgentBase::Draw(sf::RenderWindow& window) {
 
    if (goalActif == GOBGoal::Poursuivre)
        forme.setFillColor(sf::Color(255, 30,  30));   // rouge vif  = poursuite
    else if (goalActif == GOBGoal::RepondreAlerte)
        forme.setFillColor(sf::Color(255, 180,  0));   // orange     = alerte
    else if (goalActif == GOBGoal::AllerEnPause)
        forme.setFillColor(sf::Color(30,  180, 255));  // cyan       = pause
    else if (goalActif == GOBGoal::JaserCollegue)
        forme.setFillColor(sf::Color(80,  255,  80));  // vert       = jase
    else
        forme.setFillColor(sf::Color(130, 130, 130));  // GRIS = patrouille
    // (was dark red before which looked exactly like pursuit red — confusing!)
 
    window.draw(forme);
}
 

void AgentBase::DrawRayCast(sf::RenderWindow& window, Grid& grid) {
    const float PI   = 3.14159265f;
    float angleDebut = angleFace - FOV / 2.0f;

    sf::Color couleurRayon = joueurVisible
        ? sf::Color(255, 60, 60, 100)
        : sf::Color(255, 255, 200, 30);

    int nombreRayons = 60;
    for (int i = 0; i < nombreRayons; i++) {
        float angle = angleDebut + (FOV / (float)nombreRayons) * i;

        sf::Vector2f direction;
        direction.x = std::cos(angle * PI / 180.f);
        direction.y = std::sin(angle * PI / 180.f);

        RayHit resultat = CastRay(grid, position, direction, 300.0f);

        sf::Vertex ligne[2];
        ligne[0].position = position;
        ligne[0].color    = couleurRayon;
        ligne[1].position = resultat.point;
        ligne[1].color    = sf::Color(couleurRayon.r, couleurRayon.g, couleurRayon.b, 0);

        window.draw(ligne, 2, sf::PrimitiveType::Lines);
    }
}

void AgentBase::DrawPatrolRoute(sf::RenderWindow& window) const {
    if (pointsPatrouille.size() < 2) return;
 
    sf::Color lineColor(150, 150, 255, 55);  // subtle blue, mostly transparent
    sf::Color dotColor (150, 150, 255, 110);
 
    int n = (int)pointsPatrouille.size();
 
    for (int i = 0; i < n; i++) {
        sf::Vector2f from = pointsPatrouille[i];
        sf::Vector2f to   = pointsPatrouille[(i + 1) % n]; // loops back to start
 
        // Line segment
        sf::Vertex line[2];
        line[0].position = from;
        line[0].color    = lineColor;
        line[1].position = to;
        line[1].color    = lineColor;
        window.draw(line, 2, sf::PrimitiveType::Lines);
 
        // Waypoint dot
        sf::CircleShape dot(4.f);
        dot.setFillColor(dotColor);
        dot.setOrigin({ 4.f, 4.f });
        dot.setPosition(from);
        window.draw(dot);
    }
}