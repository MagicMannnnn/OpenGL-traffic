#pragma once

#include <vector>
#include <queue>
#include <glm/glm.hpp>
#include <random.hpp>
#include <settings.hpp>
#include <road_generator.hpp>
#include <unordered_map>

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

    // Find a path between start and end (returns list of positions in grid)
    std::vector<glm::ivec2> findPath(glm::ivec2 start, glm::ivec2 end) const {
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


private:
    std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT> grid;

    bool inBounds(const glm::ivec2& p) const {
        return p.x >= 0 && p.y >= 0 &&
            p.x < Settings::GRID_WIDTH && p.y < Settings::GRID_HEIGHT;
    }
};
