#include "Pathfinder.h"

std::vector<sf::Vector2f> Pathfinder::FindPath(Grid& grid, sf::Vector2f startPos, sf::Vector2f targetPos)
{
    std::vector<sf::Vector2f> emptyPath;
    float tileSize = grid.getTileSize();

    // Convertir les positions monde en coordonnees de grille
    int startX  = static_cast<int>(startPos.x  / tileSize);
    int startY  = static_cast<int>(startPos.y  / tileSize);
    int targetX = static_cast<int>(targetPos.x / tileSize);
    int targetY = static_cast<int>(targetPos.y / tileSize);

    Node* startNode  = grid.getNode(startX,  startY);
    Node* targetNode = grid.getNode(targetX, targetY);

    // Verifier que le depart et la cible sont valides
    if (startNode == nullptr || targetNode == nullptr || targetNode->isObstacle)
        return emptyPath;

    grid.ResetNodes();

    std::vector<Node*> openSet;   // noeuds a evaluer
    std::vector<Node*> closedSet; // noeuds deja evalues

    openSet.push_back(startNode);

    while (!openSet.empty())
    {
        // Trouver le noeud avec le plus petit fCost dans l'openSet
        Node* currentNode = openSet[0];
        for (int i = 1; i < (int)openSet.size(); i++)
        {
            bool lowerFCost = openSet[i]->getFCost() < currentNode->getFCost();
            bool sameFCostLowerHCost = openSet[i]->getFCost() == currentNode->getFCost()
                                    && openSet[i]->hCost < currentNode->hCost;

            if (lowerFCost || sameFCostLowerHCost)
                currentNode = openSet[i];
        }

        // Retirer le noeud courant de l'openSet
        for (int i = 0; i < (int)openSet.size(); i++)
        {
            if (openSet[i] == currentNode)
            {
                openSet.erase(openSet.begin() + i);
                break;
            }
        }

        closedSet.push_back(currentNode);

        // Si on a atteint la cible, reconstruire le chemin
        if (currentNode == targetNode)
            return RetracePath(startNode, targetNode, grid);

        // Evaluer les voisins
        std::vector<Node*> neighbors = grid.getNeighbors(currentNode);

        for (int i = 0; i < (int)neighbors.size(); i++)
        {
            Node* neighbor = neighbors[i];

            // Ignorer les obstacles et les noeuds deja evalues
            if (neighbor->isObstacle)
                continue;

            bool inClosedSet = false;
            for (int j = 0; j < (int)closedSet.size(); j++)
            {
                if (closedSet[j] == neighbor)
                {
                    inClosedSet = true;
                    break;
                }
            }
            if (inClosedSet)
                continue;

            // Calculer le nouveau cout
            float newCost = currentNode->gCost + GetDistance(currentNode, neighbor);

            bool inOpenSet = false;
            for (int j = 0; j < (int)openSet.size(); j++)
            {
                if (openSet[j] == neighbor)
                {
                    inOpenSet = true;
                    break;
                }
            }

            if (newCost < neighbor->gCost || !inOpenSet)
            {
                neighbor->gCost  = newCost;
                neighbor->hCost  = GetDistance(neighbor, targetNode);
                neighbor->parent = currentNode;

                if (!inOpenSet)
                    openSet.push_back(neighbor);
            }
        }
    }

    // Aucun chemin trouve
    return emptyPath;
}

std::vector<sf::Vector2f> Pathfinder::RetracePath(Node* startNode, Node* endNode, Grid& grid)
{
    std::vector<sf::Vector2f> waypoints;
    Node* currentNode = endNode;
    float tileSize = grid.getTileSize();

    // Remonter les parents du noeud final jusqu'au depart
    while (currentNode != startNode)
    {
        float wx = (currentNode->gridX * tileSize) + (tileSize / 2.0f);
        float wy = (currentNode->gridY * tileSize) + (tileSize / 2.0f);
        waypoints.push_back(sf::Vector2f(wx, wy));
        currentNode = currentNode->parent;
    }

    // Inverser pour avoir le chemin du depart vers la cible
    for (int i = 0; i < (int)waypoints.size() / 2; i++)
    {
        sf::Vector2f temp = waypoints[i];
        waypoints[i] = waypoints[waypoints.size() - 1 - i];
        waypoints[waypoints.size() - 1 - i] = temp;
    }

    return waypoints;
}

float Pathfinder::GetDistance(Node* nodeA, Node* nodeB)
{
    int dstX = std::abs(nodeA->gridX - nodeB->gridX);
    int dstY = std::abs(nodeA->gridY - nodeB->gridY);

    // Deplacement diagonal = 14, horizontal/vertical = 10
    if (dstX > dstY)
        return 14.f * dstY + 10.f * (dstX - dstY);
    else
        return 14.f * dstX + 10.f * (dstY - dstX);
}