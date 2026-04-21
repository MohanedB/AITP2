#include "LevelGenerator.h"
#include "../AI/Pathfinder.h"
#include <cmath>
#include <cstdlib>
#include <ctime>


static const int NUM_ROOMS = 15;
static const int ROOM_SIZE = 5;

float LevelGenerator::Dist(sf::Vector2f a, sf::Vector2f b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

// Returns the centre pixel of a tile (tx, ty)
sf::Vector2f LevelGenerator::TileCenter(int tx, int ty, float ts) {
    return sf::Vector2f(tx * ts + ts * 0.5f, ty * ts + ts * 0.5f);
}

// Returns the centre pixel of every tile that is not a wall
std::vector<sf::Vector2f> LevelGenerator::GetOpenTiles(Grid& grid) {
    std::vector<sf::Vector2f> result;
    float ts = grid.getTileSize();

    for (int x = 0; x < grid.getWidth(); x++) {
        for (int y = 0; y < grid.getHeight(); y++) {
            Node* n = grid.getNode(x, y);
            if (n != nullptr && !n->isObstacle)
                result.push_back(TileCenter(x, y, ts));
        }
    }
    return result;
}

void LevelGenerator::CarveMaze(Grid& grid) {
    int w     = grid.getWidth();
    int h     = grid.getHeight();
    int cellW = w / 2;   // number of cell columns
    int cellH = h / 2;   // number of cell rows

    // Track which cells we have already opened
    std::vector<std::vector<bool>> visited(cellW, std::vector<bool>(cellH, false));

    // Manual stack (we push cells we're exploring, pop when stuck)
    std::vector<std::pair<int, int>> stack;

    int dx[] = { 1,  0, -1,  0 };
    int dy[] = { 0,  1,  0, -1 };

    // Open the first cell and add it to the stack
    grid.SetObstacle(1, 1, false);
    visited[0][0] = true;
    stack.push_back(std::make_pair(0, 0));

    while (!stack.empty()) {
        int cx = stack.back().first;
        int cy = stack.back().second;

        // Find all unvisited neighbours
        std::vector<int> dirs;
        for (int d = 0; d < 4; d++) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];
            bool inBounds = (nx >= 0 && nx < cellW && ny >= 0 && ny < cellH);
            if (inBounds && !visited[nx][ny])
                dirs.push_back(d);
        }

        if (dirs.empty()) {
            stack.pop_back(); // dead end — backtrack
            continue;
        }

        // Pick a random direction
        int d  = dirs[rand() % (int)dirs.size()];
        int nx = cx + dx[d];
        int ny = cy + dy[d];

        // Remove the wall between current cell and the chosen neighbour
        grid.SetObstacle(1 + 2 * cx + dx[d], 1 + 2 * cy + dy[d], false);

        // Open the neighbour cell itself
        grid.SetObstacle(1 + 2 * nx, 1 + 2 * ny, false);

        visited[nx][ny] = true;
        stack.push_back(std::make_pair(nx, ny));
    }
}


void LevelGenerator::AddRooms(Grid& grid) {
    int w    = grid.getWidth();
    int h    = grid.getHeight();
    int half = ROOM_SIZE / 2;

    for (int r = 0; r < NUM_ROOMS; r++) {
        // Random centre tile for this room, kept away from the border
        int cx = (half + 1) + rand() % (w - ROOM_SIZE - 2);
        int cy = (half + 1) + rand() % (h - ROOM_SIZE - 2);

        // Open every tile in the ROOM_SIZE x ROOM_SIZE block
        for (int ddx = -half; ddx <= half; ddx++)
            for (int ddy = -half; ddy <= half; ddy++)
                grid.SetObstacle(cx + ddx, cy + ddy, false);
    }
}



// Closest open tile to the top-left corner — player start
sf::Vector2f LevelGenerator::PlaceSpawn(Grid& grid, const std::vector<sf::Vector2f>& open) {
    sf::Vector2f corner = TileCenter(1, 1, grid.getTileSize());
    sf::Vector2f best   = open[0];
    float        bestD  = Dist(open[0], corner);

    for (int i = 1; i < (int)open.size(); i++) {
        float d = Dist(open[i], corner);
        if (d < bestD) { bestD = d; best = open[i]; }
    }
    return best;
}

// Open tile farthest from a given position — used for the exit
sf::Vector2f LevelGenerator::PlaceFarthest(sf::Vector2f from,
                                            const std::vector<sf::Vector2f>& open)
{
    sf::Vector2f best  = open[0];
    float        bestD = -1.f;

    for (int i = 0; i < (int)open.size(); i++) {
        float d = Dist(open[i], from);
        if (d > bestD) { bestD = d; best = open[i]; }
    }
    return best;
}

// Open tile roughly halfway between spawn and exit — used for the key
sf::Vector2f LevelGenerator::PlaceMiddle(sf::Vector2f spawn, sf::Vector2f exit,
                                          const std::vector<sf::Vector2f>& open)
{
    float        total     = Dist(spawn, exit);
    sf::Vector2f best      = open[open.size() / 2]; // safe default
    float        bestScore = -1.f;

    for (int i = 0; i < (int)open.size(); i++) {
        float d = Dist(open[i], spawn);

        // Only consider tiles in the 35%–65% range of spawn-to-exit distance
        if (d > total * 0.35f && d < total * 0.65f) {
            float score = Dist(open[i], exit);
            if (score > bestScore) { bestScore = score; best = open[i]; }
        }
    }
    return best;
}



std::vector<sf::Vector2f> LevelGenerator::MakePatrol(sf::Vector2f spawn,
                                                       const std::vector<sf::Vector2f>& open)
{
    std::vector<sf::Vector2f> route;
    route.push_back(spawn);

    for (int i = 1; i < 4; i++) {
        float        bestDist = -1.f;
        sf::Vector2f bestTile = spawn;

        for (int j = 0; j < (int)open.size(); j++) {
            // Keep patrol points local — within 300px of this agent's spawn
            if (Dist(open[j], spawn) > 300.f) continue;

            // How far is this tile from the closest already-picked point?
            float closest = 9999.f;
            for (int k = 0; k < (int)route.size(); k++) {
                float d = Dist(open[j], route[k]);
                if (d < closest) closest = d;
            }

            if (closest > bestDist) { bestDist = closest; bestTile = open[j]; }
        }

        if (bestDist > 5.f)
            route.push_back(bestTile);
    }

    return route;
}



LevelData LevelGenerator::Generate(Grid& grid, int numAgents) {
    srand((unsigned int)time(nullptr));

    float ts = grid.getTileSize();

    // 1. Reset everything to walls
    grid.SetAllObstacles();

    // 2. Carve the DFS maze
    CarveMaze(grid);

    // 3. Carve open rooms on top
    AddRooms(grid);

    // 4. Get every open tile
    std::vector<sf::Vector2f> open = GetOpenTiles(grid);
    if (open.empty()) return LevelData(); // should never happen

    // 5. Place player, exit, key
    LevelData data;
    data.playerSpawn = PlaceSpawn(grid, open);
    data.exitPos     = PlaceFarthest(data.playerSpawn, open);
    data.keyPos      = PlaceMiddle(data.playerSpawn, data.exitPos, open);

    // 6. Break room = closest open tile to the map centre
    sf::Vector2f mapCenter(grid.getWidth() * ts * 0.5f, grid.getHeight() * ts * 0.5f);
    data.breakRoomPos  = open[0];
    float closestToCenter = Dist(open[0], mapCenter);
    for (int i = 1; i < (int)open.size(); i++) {
        float d = Dist(open[i], mapCenter);
        if (d < closestToCenter) { closestToCenter = d; data.breakRoomPos = open[i]; }
    }

    // 7. Place agents
    //    Shuffle the open tiles list, then just pick the first ones
    //    that are far enough from the player (so nobody sees you at the start).
    data.agentSpawns .resize(numAgents);
    data.patrolRoutes.resize(numAgents);

    // Fisher-Yates shuffle
    for (int i = (int)open.size() - 1; i > 0; i--) {
        int          j   = rand() % (i + 1);
        sf::Vector2f tmp = open[i];
        open[i]          = open[j];
        open[j]          = tmp;
    }

    int placed = 0;
    for (int i = 0; i < (int)open.size() && placed < numAgents; i++) {
        // Skip tiles too close to the player (vision range = 300px, so 250px is safe)
        if (Dist(open[i], data.playerSpawn) < 250.f) continue;

        data.agentSpawns[placed]  = open[i];
        data.patrolRoutes[placed] = MakePatrol(open[i], open);
        placed++;
    }

    return data;
}