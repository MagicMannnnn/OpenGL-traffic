#pragma once

#include <road.hpp>
#include <Shader.h>
#include <camera.hpp>
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <random.hpp>
#include <road_generator.hpp>
#include <road_path_finder.hpp>






class RoadManager {
public:
	RoadManager()
		: m_shader(RESOURCES_PATH"shaders/road.vert", RESOURCES_PATH"shaders/road.frag"),
		m_straight(RESOURCES_PATH"textures/road.png", 1, 1),
		m_tJunction(RESOURCES_PATH"textures/t-junction.png", 1, 1),
		m_crossRoads(RESOURCES_PATH"textures/crossRoads.png", 1, 1),
		m_corner(RESOURCES_PATH"textures/corner.png", 1, 1),
		m_path(RESOURCES_PATH"textures/red2.png", 1, 1),
		m_floor(RESOURCES_PATH"textures/floor.png", Settings::GRID_WIDTH, Settings::GRID_HEIGHT)
	{
		m_floorTile = RoadTile(glm::vec3(0, -0.1f, 0), 0);

		RoadGenerator::generate(m_gridIntegers);
		RoadGenerator::generateTiles(m_gridIntegers, m_grid);

		m_pathFinder = RoadPathfinder(m_gridIntegers);
	}
	~RoadManager() {
		m_pathTiles.clear();
	}

	void reGenerate() {
		m_pathTiles.clear();
		for (std::array<RoadTile, Settings::GRID_HEIGHT>&row : m_grid) {
			for (RoadTile& tile : row) {
				tile.type = NONE;
			}
		}
		RoadGenerator::generate(m_gridIntegers);
		RoadGenerator::generateTiles(m_gridIntegers, m_grid);

		m_pathFinder = RoadPathfinder(m_gridIntegers);
	}

	void demoPath() {

		std::cout << "\nfinding path...\n";	
		std::vector<glm::ivec2> path = m_pathFinder.findPathThroughCheckpoint();

		if (path.empty()) return;
		std::cout << "found path! \n";
		m_pathTiles.clear();

		for (const auto& coord : path) {
			glm::vec3 pos(
				coord.x - (float)(Settings::GRID_WIDTH - 1) / 2.0f,
				0.01f,
				coord.y - (float)(Settings::GRID_HEIGHT - 1) / 2.0f
			);

			
			m_pathTiles.emplace_back(pos, 0.0f, PATH);
		}
	}

	void Render(glm::mat4& projection, Camera& camera) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_shader.use();
		m_shader.setMat4("projection", projection);
		m_shader.setMat4("view", camera.GetViewMatrix());

		m_floor.setupRender(m_shader);
		m_shader.setMat4("model", m_floorTile.model);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		m_crossRoads.setupRender(m_shader);
		for (std::array<RoadTile, Settings::GRID_HEIGHT>&row : m_grid) {
			for (RoadTile& road : row) {
				if (road.type != CROSSROADS) continue;
				m_shader.setMat4("model", road.model);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			}
		}

		m_straight.setupRender(m_shader);
		for (std::array<RoadTile, Settings::GRID_HEIGHT>&row : m_grid) {
			for (RoadTile& road : row) {
				if (road.type != STRAIGHT) continue;
				m_shader.setMat4("model", road.model);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			}
		}

		m_corner.setupRender(m_shader);
		for (std::array<RoadTile, Settings::GRID_HEIGHT>&row : m_grid) {
			for (RoadTile& road : row) {
				if (road.type != CORNER) continue;
				m_shader.setMat4("model", road.model);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			}
		}

		m_tJunction.setupRender(m_shader);
		for (std::array<RoadTile, Settings::GRID_HEIGHT>&row : m_grid) {
			for (RoadTile& road : row) {
				if (road.type != TJUNCTION) continue;
				m_shader.setMat4("model", road.model);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			}
		}
		
		m_path.setupRender(m_shader);
		for (RoadTile& road : m_pathTiles) {
			if (road.type != PATH) continue;
			m_shader.setMat4("model", road.model);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		}
		

		glBindVertexArray(0);
	}

private:
	Shader m_shader;
	Road m_straight;
	Road m_tJunction;
	Road m_crossRoads;
	Road m_corner;
	Road m_floor;
	Road m_path;
	RoadTile m_floorTile;

	std::array<std::array<RoadTile, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT> m_grid;
	std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT> m_gridIntegers;

	RoadPathfinder m_pathFinder;
	std::vector<RoadTile> m_pathTiles;
};