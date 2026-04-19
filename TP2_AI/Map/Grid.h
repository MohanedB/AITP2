#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct Node {
    int   gridX;
    int   gridY;
    bool  isObstacle;

    float  gCost  = 0;
    float  hCost  = 0;
    Node*  parent = nullptr;

    Node(int x, int y, bool obstacle = false)
        : gridX(x), gridY(y), isObstacle(obstacle) {}

    float getFCost() const { return gCost + hCost; }
};

class Grid {
private:
    int   width;
    int   height;
    float tileSize;
    std::vector<std::vector<Node>> nodes;

public:
    Grid(int w, int h, float size);

    // Called once at construction — now empty (LevelGenerator does the work)
    void GenerateLevel();

    void SetObstacle(int x, int y, bool isObstacle);

    // NEW — set every tile to obstacle (called before maze generation)
    void SetAllObstacles();

    float getTileSize() const { return tileSize; }

    // NEW — needed by LevelGenerator
    int getWidth()  const { return width;  }
    int getHeight() const { return height; }

    Node*              getNode(int x, int y);
    std::vector<Node*> getNeighbors(Node* node);
    void               ResetNodes();

    void Draw(sf::RenderWindow& window);
};