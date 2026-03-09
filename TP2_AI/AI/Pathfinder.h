#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "../Map/Grid.h"

class Pathfinder
{
public:
    static std::vector<sf::Vector2f> FindPath(Grid& grid, sf::Vector2f startPos, sf::Vector2f targetPos);

private:
    static float GetDistance(Node* nodeA, Node* nodeB);
    static std::vector<sf::Vector2f> RetracePath(Node* startNode, Node* endNode, Grid& grid);
};