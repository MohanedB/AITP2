#include "Pathfinder.h"
#include <cmath>
#include <algorithm>

std::vector<sf::Vector2f> Pathfinder::FindPath(Grid& grid, sf::Vector2f startPos, sf::Vector2f targetPos) {
    std::vector<sf::Vector2f> path;
    float tileSize = grid.getTileSize();

    Node* startNode = grid.getNode(static_cast<int>(startPos.x / tileSize), static_cast<int>(startPos.y / tileSize));
    Node* targetNode = grid.getNode(static_cast<int>(targetPos.x / tileSize), static_cast<int>(targetPos.y / tileSize));

    if (!startNode || !targetNode || targetNode->isObstacle) return path;

    grid.ResetNodes();
    std::vector<Node*> openSet;
    std::vector<Node*> closedSet;
    openSet.push_back(startNode);

    while (!openSet.empty()) {
        Node* currentNode = openSet[0];
        for (size_t i = 1; i < openSet.size(); i++) {
            if (openSet[i]->getFCost() < currentNode->getFCost() || (openSet[i]->getFCost() == currentNode->getFCost() && openSet[i]->hCost < currentNode->hCost)) {
                currentNode = openSet[i];
            }
        }

        auto it = std::find(openSet.begin(), openSet.end(), currentNode);
        openSet.erase(it);
        closedSet.push_back(currentNode);

        if (currentNode == targetNode) return RetracePath(startNode, targetNode, grid);

        for (Node* neighbor : grid.getNeighbors(currentNode)) {
            if (neighbor->isObstacle || std::find(closedSet.begin(), closedSet.end(), neighbor) != closedSet.end()) continue;

            float newMovementCostToNeighbor = currentNode->gCost + GetDistance(currentNode, neighbor);
            bool inOpenSet = std::find(openSet.begin(), openSet.end(), neighbor) != openSet.end();

            if (newMovementCostToNeighbor < neighbor->gCost || !inOpenSet) {
                neighbor->gCost = newMovementCostToNeighbor;
                neighbor->hCost = GetDistance(neighbor, targetNode);
                neighbor->parent = currentNode;
                if (!inOpenSet) openSet.push_back(neighbor);
            }
        }
    }
    return path;
}

std::vector<sf::Vector2f> Pathfinder::RetracePath(Node* startNode, Node* endNode, Grid& grid) {
    std::vector<sf::Vector2f> waypoints;
    Node* currentNode = endNode;
    float tSize = grid.getTileSize();

    while (currentNode != startNode) {
        waypoints.push_back({ (currentNode->gridX * tSize) + (tSize / 2.0f), (currentNode->gridY * tSize) + (tSize / 2.0f) });
        currentNode = currentNode->parent;
    }
    std::reverse(waypoints.begin(), waypoints.end());
    return waypoints;
}

float Pathfinder::GetDistance(Node* nodeA, Node* nodeB) {
    int dstX = std::abs(nodeA->gridX - nodeB->gridX);
    int dstY = std::abs(nodeA->gridY - nodeB->gridY);
    if (dstX > dstY) return 14.f * dstY + 10.f * (dstX - dstY);
    return 14.f * dstX + 10.f * (dstY - dstX);
}