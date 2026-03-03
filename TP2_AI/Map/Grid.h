#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// Structure d'un Nœud mise à jour pour le graphe de navigation A*
struct Node {
    int gridX;
    int gridY;
    bool isObstacle;
    
    // Variables pour l'algorithme A*
    float gCost = 0; // Coût du départ au nœud actuel
    float hCost = 0; // Estimation du coût du nœud à l'arrivée (Heuristique)
    Node* parent = nullptr;

    Node(int x, int y, bool obstacle = false) 
        : gridX(x), gridY(y), isObstacle(obstacle) {}

    // FCost = G + H (Priorité totale)
    float getFCost() const { return gCost + hCost; }
};

class Grid {
private:
    int width;
    int height;
    float tileSize;
    std::vector<std::vector<Node>> nodes;

public:
    Grid(int w, int h, float size);

    void GenerateLevel();
    void SetObstacle(int x, int y, bool isObstacle);

    float getTileSize() const { return tileSize; }
    Node* getNode(int x, int y);

    std::vector<Node*> getNeighbors(Node* node);
    void ResetNodes();

    void Draw(sf::RenderWindow& window);
};