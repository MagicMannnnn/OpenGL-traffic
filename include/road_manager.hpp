#pragma once

#include <road.hpp>
#include <Shader.h>
#include <camera.hpp>
#include <vector>
#include <glm/glm.hpp>


enum ROADTYPE {
	STRAIGHT = 0,
	CORNER
};

struct RoadTile {
	glm::mat4 model;
	glm::vec3 position;
	float angle;
	ROADTYPE type;

	RoadTile() = default;
	RoadTile(glm::vec3 _position, float _angle, ROADTYPE _type=STRAIGHT) {
		position = _position;
		type = _type;
		angle = _angle;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)); // Apply rotation to identity matrix
		model = glm::translate(model, position); // Apply the translation afterward
	};

};




class RoadManager {
public:
	RoadManager()
		: m_shader(RESOURCES_PATH"shaders/road.vert", RESOURCES_PATH"shaders/road.frag"),
		m_straight(RESOURCES_PATH"textures/road.png", 1, 1),
		m_floor(RESOURCES_PATH"textures/floor.png", 10, 10)
	{
		m_floorTile = RoadTile(glm::vec3(0, -0.1f, 0), 0);

		m_roads.push_back(RoadTile(glm::vec3(0, 0, 0), 0));
		m_roads.push_back(RoadTile(glm::vec3(1, 0, 0), 0));
		m_roads.push_back(RoadTile(glm::vec3(2, 0, 0), 0));
	}
	~RoadManager() {
		m_roads.clear();
	}

	void Render(glm::mat4& projection, Camera& camera) {
		m_shader.use();
		m_shader.setMat4("projection", projection);
		m_shader.setMat4("view", camera.GetViewMatrix());

		m_floor.setupRender(m_shader);
		m_shader.setMat4("model", m_floorTile.model);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//straight roads
		m_straight.setupRender(m_shader);
		for (RoadTile& road : m_roads) {
			if (road.type != STRAIGHT) continue;

			m_shader.setMat4("model", road.model);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		}

		glBindVertexArray(0);
	}

private:
	Shader m_shader;
	Road m_straight;
	Road m_floor;
	RoadTile m_floorTile;
	std::vector<RoadTile> m_roads;
};