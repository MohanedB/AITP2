#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// Structure d'un Nœud pour l'algorithme A*
struct Node {
    int gridX;
    int gridY;
    bool isObstacle;
    
    // Variables requises pour le pathfinding
    float gCost = 0;
    float hCost = 0;
    Node* parent = nullptr;

    Node(int x, int y, bool obstacle = false) 
        : gridX(x), gridY(y), isObstacle(obstacle) {}

    float getFCost() const { return gCost + hCost; }
};

class Grid {
private:
    int width;
    int height;
    float tileSize;
    std::vector<std::vector<Node>> nodes;

public:
    // Constructeur
    Grid(int w, int h, float size);

    // Initialisation
    void GenerateLevel();
    void SetObstacle(int x, int y, bool isObstacle);

    // Accesseurs
    float getTileSize() const { return tileSize; } 
    Node* getNode(int x, int y);

    // Fonctions pour l'algorithme A*
    std::vector<Node*> getNeighbors(Node* node);
    void ResetNodes();

    // Rendu visuel
    void Draw(sf::RenderWindow& window);
};