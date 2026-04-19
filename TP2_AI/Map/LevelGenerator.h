#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "../Map/Grid.h"

struct LevelData {
    sf::Vector2f playerSpawn;
    sf::Vector2f keyPos;
    sf::Vector2f exitPos;
    sf::Vector2f breakRoomPos;
    std::vector<sf::Vector2f>              agentSpawns;
    std::vector<std::vector<sf::Vector2f>> patrolRoutes;
};

class LevelGenerator {
public:
    static LevelData Generate(Grid& grid, int numAgents = 10);

private:
    // Map generation
    static void CarveMaze(Grid& grid);
    static void AddRooms (Grid& grid);

    // Helpers
    static std::vector<sf::Vector2f> GetOpenTiles(Grid& grid);
    static sf::Vector2f              TileCenter(int tx, int ty, float ts);
    static float                     Dist(sf::Vector2f a, sf::Vector2f b);

    // Placement
    static sf::Vector2f PlaceSpawn   (Grid& grid, const std::vector<sf::Vector2f>& open);
    static sf::Vector2f PlaceFarthest(sf::Vector2f from, const std::vector<sf::Vector2f>& open);
    static sf::Vector2f PlaceMiddle  (sf::Vector2f spawn, sf::Vector2f exit,
                                      const std::vector<sf::Vector2f>& open);

    // Patrol routes
    static std::vector<sf::Vector2f> MakePatrol(sf::Vector2f spawn,
                                                 const std::vector<sf::Vector2f>& open);
};