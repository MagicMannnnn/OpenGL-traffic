#pragma once

#include <road_manager.hpp>
#include <vector>
#include <car.hpp>
#include <obj_loader.hpp>

class CarManager {
public:
	CarManager() : 
		m_shader(RESOURCES_PATH"shaders/instanced.vert", RESOURCES_PATH"shaders/instanced.frag")
	
	{
		m_models[STANDARD] = Model((RESOURCES_PATH"models/car/LowPolyCars.obj"));
		car = Car(glm::vec3(0, 0.0f, 0), 0.f, STANDARD);
	}
	~CarManager() {
		m_carInstanceMatrices.clear();
	}

	void addCar(std::vector<RoadTile>& route) {

	}

	void Render(glm::mat4& projection, Camera& camera) {
		m_carInstanceMatrices.clear();
		m_shader.use();
		m_shader.setMat4("projection", projection);
		m_shader.setMat4("view", camera.GetViewMatrix());
		m_shader.setVec3("lightPos", glm::vec3(10.0f, 100.0f, 10.0f)); // Adjust light position
		m_shader.setVec3("viewPos", camera.Position);
		m_shader.setVec3("lightColor", glm::vec3(0.0f, 1.0f, 0.0f)); // White light

		m_carInstanceMatrices.push_back(glm::scale(car.model, glm::vec3(0.1f))); // Scale down to 10%);
		
		m_models[STANDARD].setupInstancing(m_carInstanceMatrices);
		m_models[STANDARD].DrawInstanced(m_shader, m_carInstanceMatrices.size());
		glBindVertexArray(0);
	}

private:
	std::vector<glm::mat4> m_carInstanceMatrices;
	Shader m_shader;
	std::array < Model, 1> m_models;
	Car car;
};