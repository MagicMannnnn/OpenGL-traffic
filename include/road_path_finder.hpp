#pragma once

#include <vector>
#include <queue>
#include <glm/glm.hpp>
#include <random.hpp>
#include <settings.hpp>
#include <road_generator.hpp>
#include <unordered_map>
#include <set>

struct IVec2Hasher {
    std::size_t operator()(const glm::ivec2& v) const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};

class RoadPathfinder {
public:
    RoadPathfinder() = default;
    RoadPathfinder(const std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& _grid)
        : grid(_grid) {}

    // Generate a random edge point that's on a road (grid[y][x] > 0)
    glm::ivec2 getRandomEdgePoint() const {
        for (int attempts = 0; attempts < 100; ++attempts) {
            int edge = Random::getRangeInt(0, 3);
            int x = 0, y = 0;

            switch (edge) {
            case 0:  // Top
                x = Random::getRangeInt(0, Settings::GRID_WIDTH - 1);
                y = 0;
                break;
            case 1:  // Right
                x = Settings::GRID_WIDTH - 1;
                y = Random::getRangeInt(0, Settings::GRID_HEIGHT - 1);
                break;
            case 2:  // Bottom
                x = Random::getRangeInt(0, Settings::GRID_WIDTH - 1);
                y = Settings::GRID_HEIGHT - 1;
                break;
            case 3:  // Left
                x = 0;
                y = Random::getRangeInt(0, Settings::GRID_HEIGHT - 1);
                break;
            }

            if (grid[y][x] > 0) {
                return glm::ivec2(x, y);
            }
        }

        // Fallback: return center if no edge road found
        return glm::ivec2(-1,-1);
    }

    glm::ivec2 getRandomEdgePoint(int edge) const {
        for (int attempts = 0; attempts < 100; ++attempts) {
            int x = 0, y = 0;

            switch (edge) {
            case 0:  // Top
                x = Random::getRangeInt(0, Settings::GRID_WIDTH - 1);
                y = 0;
                break;
            case 1:  // Right
                x = Settings::GRID_WIDTH - 1;
                y = Random::getRangeInt(0, Settings::GRID_HEIGHT - 1);
                break;
            case 2:  // Bottom
                x = Random::getRangeInt(0, Settings::GRID_WIDTH - 1);
                y = Settings::GRID_HEIGHT - 1;
                break;
            case 3:  // Left
                x = 0;
                y = Random::getRangeInt(0, Settings::GRID_HEIGHT - 1);
                break;
            }

            if (grid[y][x] > 0) {
                return glm::ivec2(x, y);
            }
        }

        // Fallback: return center if no edge road found
        return glm::ivec2(-1, -1);
    }


    glm::ivec2 getRandomPoint() const {
        
        for (int i = 0; i < 100; i++) {
            int x = Random::getRangeInt(0, Settings::GRID_WIDTH - 1);
            int y = Random::getRangeInt(0, Settings::GRID_HEIGHT - 1);
            if (grid[y][x] > 0) {
                return glm::ivec2(x, y);
            }
        }

        return glm::ivec2(-1, -1);

    }


    // Find a path between start and end (returns list of positions in grid)
    std::vector<glm::ivec2> findPath(glm::ivec2 start, glm::ivec2 end) const {
        if (!inBounds(start) || !inBounds(end)) {
            std::cout << "start: " << start.x << ", " << start.y << "\n";
            std::cout << "end: " << end.x << ", " << end.y << "\n";
            return {};
        }
        if (grid[start.y][start.x] == 0 || grid[end.y][end.x] == 0)
            return {};

        std::queue<glm::ivec2> frontier;
        std::unordered_map<glm::ivec2, glm::ivec2, IVec2Hasher> cameFrom;
        std::vector<std::vector<bool>> visited(Settings::GRID_HEIGHT, std::vector<bool>(Settings::GRID_WIDTH, false));

        frontier.push(start);
        visited[start.y][start.x] = true;

        const glm::ivec2 directions[4] = {
            {0, -1}, {1, 0}, {0, 1}, {-1, 0}
        };

        while (!frontier.empty()) {
            glm::ivec2 current = frontier.front();
            frontier.pop();

            if (current == end) break;

            for (auto dir : directions) {
                glm::ivec2 neighbor = current + dir;
                if (inBounds(neighbor) && !visited[neighbor.y][neighbor.x] && grid[neighbor.y][neighbor.x] > 0) {
                    visited[neighbor.y][neighbor.x] = true;
                    frontier.push(neighbor);
                    cameFrom[neighbor] = current;
                }
            }
        }

        // Reconstruct path
        std::vector<glm::ivec2> path;
        if (cameFrom.find(end) == cameFrom.end()) return path; // no path

        glm::ivec2 current = end;
        while (current != start) {
            path.push_back(current);
            current = cameFrom[current];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
        return path;
    }

    std::vector<glm::ivec2> findPath(glm::ivec2 start, glm::ivec2 end, glm::ivec2 blockPoint) const {
        if (!inBounds(start) || !inBounds(end)) return {};
        if (grid[start.y][start.x] == 0 || grid[end.y][end.x] == 0) return {};
        if (start == blockPoint || end == blockPoint) return {}; // can't start or end at blocked point

        std::queue<glm::ivec2> frontier;
        std::unordered_map<glm::ivec2, glm::ivec2, IVec2Hasher> cameFrom;
        std::vector<std::vector<bool>> visited(Settings::GRID_HEIGHT, std::vector<bool>(Settings::GRID_WIDTH, false));

        frontier.push(start);
        visited[start.y][start.x] = true;

        const glm::ivec2 directions[4] = {
            {0, -1}, {1, 0}, {0, 1}, {-1, 0}
        };

        while (!frontier.empty()) {
            glm::ivec2 current = frontier.front();
            frontier.pop();

            if (current == end) break;

            for (auto dir : directions) {
                glm::ivec2 neighbor = current + dir;
                if (neighbor == blockPoint) continue;

                if (inBounds(neighbor) && !visited[neighbor.y][neighbor.x] &&
                    grid[neighbor.y][neighbor.x] > 0) {

                    visited[neighbor.y][neighbor.x] = true;
                    frontier.push(neighbor);
                    cameFrom[neighbor] = current;
                }
            }
        }

        // Reconstruct path
        std::vector<glm::ivec2> path;
        if (cameFrom.find(end) == cameFrom.end()) return path; // no path

        glm::ivec2 current = end;
        while (current != start) {
            path.push_back(current);
            current = cameFrom[current];
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
        return path;
    }



    std::vector<glm::ivec2> findPathThroughCheckpoint() {

        int totalAttemps = 0;

        std::vector<glm::ivec2> path;


        while (path.empty() && totalAttemps < 5) {
            glm::ivec2 start = getRandomEdgePoint();
            int attempts = 0;
            while (start == glm::ivec2(-1, -1) && attempts < 5) {
                start = getRandomEdgePoint();
            }
            if (start == glm::ivec2(-1, -1)) continue;


            glm::ivec2 middle = getRandomPoint();
            attempts = 0;
            while (middle == glm::ivec2(-1, -1) && attempts < 5) {
                middle = getRandomPoint();
                attempts++;
            }
            if (middle == glm::ivec2(-1, -1)) {
                std::cout << "\nmiddle failed\n";
                continue;
            }

            attempts = 0;
            glm::ivec2 end = getRandomEdgePoint();

            while (start == end && attempts < 10 && end != glm::ivec2(-1, -1)) {
                end = getRandomEdgePoint();
                attempts++;
            }
            if (start == end || end == glm::ivec2(-1, -1)) {
                continue;
            }

            std::vector<glm::ivec2> pathA = findPath(start, middle);
            std::vector<glm::ivec2> pathB;
            if (pathA.size() > 2) {
                pathB = findPath(middle, end, pathA[pathA.size()-2]);
            }

            if (pathA.size() > 2 && pathB.size() > 2) {
                path.insert(path.begin(), pathA.begin(), pathA.end()-1);
                path.insert(path.end(), pathB.begin(), pathB.end());
                return path;
            }

            totalAttemps++;
        }

        std::cout << "couldnt find path with checkpoint, falling back to opposite edges...\n";
        return findPathOppositeEdges();
    }

    std::vector<glm::ivec2> findPathOppositeEdges() {

        int totalAttemps = 0;

        std::vector<glm::ivec2> path;

        while (path.empty() && totalAttemps < 5) {
            glm::ivec2 start = getRandomEdgePoint();
            int attempts = 0;
            while (start == glm::ivec2(-1, -1) && attempts < 5) {
                start = getRandomEdgePoint();
            }
            if (start == glm::ivec2(-1, -1)) continue;



            int edge;
            if (start.x == 0) {
                edge = 1;
            }
            else if (start.y == 0) {
                edge = 2;
            }
            else if (start.x == Settings::GRID_WIDTH - 1) {
                edge = 3;
            }
            else {
                edge = 0;
            }

            attempts = 0;
            glm::ivec2 end = getRandomEdgePoint(edge);

            while (start == end && attempts < 10 && end != glm::ivec2(-1, -1)) {
                end = getRandomEdgePoint(edge);
            }
            if (start == end) continue;

            path = findPath(start, end);

            totalAttemps++;
        }

        return path;

        
    }



private:
    std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT> grid;

    bool inBounds(const glm::ivec2& p) const {
        return p.x >= 0 && p.y >= 0 &&
            p.x < Settings::GRID_WIDTH && p.y < Settings::GRID_HEIGHT;
    }
};
