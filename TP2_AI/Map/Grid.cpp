#include "Grid.h"

Grid::Grid(int w, int h, float size) : width(w), height(h), tileSize(size) {
    // Initialisation de la grille vide
    for (int x = 0; x < width; ++x) {
        std::vector<Node> column;
        for (int y = 0; y < height; ++y) {
            column.push_back(Node(x, y, false));
        }
        nodes.push_back(column);
    }
    
    GenerateLevel();
}

void Grid::GenerateLevel() {
    // '#' = Mur (Obstacle)
    // '.' = Sol (Marchable)
    const std::vector<std::string> mapLayout = {
        "########################################",
        "#......................................#",
        "#.###########..........................#",
        "#.#.......................########.....#",
        "#.#..............................#.....#",
        "#.#.........#.............########.....#",
        "#.#.........#..........................#",
        "#.#######.###..........................#",
        "#..................#######.............#",
        "#........................#.............#",
        "#....#####......####.....#.............#",
        "#....#.............#.....#.............#",
        "#....#.............#######.............#",
        "#....#.................................#",
        "#....##############....................#",
        "#.................#....................#",
        "#.................#........###.........#",
        "########..........#.......#####........#",
        "#......#..........#......#######.......#",
        "#......############.......#####........#",
        "#..........................###.........#",
        "#......................................#",
        "#......................................#",
        "#.......#.......#......................#",
        "#.......#.......#.........######.......#",
        "#.......#.......#......................#",
        "#.......#########.........######.......#",
        "#......................................#",
        "#......................................#",
        "########################################"
    };

    // Lecture du dessin pour générer les nœuds de la carte
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (mapLayout[y][x] == '#') {
                SetObstacle(x, y, true);
            }
        }
    }
}

void Grid::SetObstacle(int x, int y, bool isObstacle) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        nodes[x][y].isObstacle = isObstacle;
    }
}

Node* Grid::getNode(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return &nodes[x][y];
    }
    return nullptr;
}

std::vector<Node*> Grid::getNeighbors(Node* node) {
    std::vector<Node*> neighbors;
    // Vérification des 8 directions autour de la tuile
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) continue; // On ignore la case centrale

            int checkX = node->gridX + x;
            int checkY = node->gridY + y;

            if (checkX >= 0 && checkX < width && checkY >= 0 && checkY < height) {
                neighbors.push_back(&nodes[checkX][checkY]);
            }
        }
    }
    return neighbors;
}

void Grid::ResetNodes() {
    // Nettoie les calculs A* pour qu'une nouvelle recherche puisse commencer de zéro
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            nodes[x][y].gCost = 0;
            nodes[x][y].hCost = 0;
            nodes[x][y].parent = nullptr;
        }
    }
}

void Grid::Draw(sf::RenderWindow& window) {
    sf::RectangleShape wallTile(sf::Vector2f(tileSize, tileSize));
    sf::RectangleShape floorTile(sf::Vector2f(tileSize - 1.0f, tileSize - 1.0f));

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (nodes[x][y].isObstacle) {
                wallTile.setPosition({ x * tileSize, y * tileSize });
                wallTile.setFillColor(sf::Color(70, 80, 95)); // Gris-bleu
                window.draw(wallTile);
            } else {
                floorTile.setPosition({ x * tileSize, y * tileSize });
                floorTile.setFillColor(sf::Color(20, 20, 25)); // Sombre
                window.draw(floorTile);
            }
        }
    }
}