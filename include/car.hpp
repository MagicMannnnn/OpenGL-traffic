#pragma once

#include <glm/glm.hpp>
#include <road_generator.hpp>

enum CARTYPE {
	STANDARD = 0,
	SIZE
};

struct Car {
	glm::vec3 position;
	glm::mat4 model;
	float angle;
	CARTYPE type = CARTYPE::SIZE;
	float speed = 2.0f; // Units per second

	std::vector<RoadTile> route;

	// Internal state
	int currentTargetIndex = 0;
	float t = 0.0f;

	Car() = default;

	Car(glm::vec3 _position, float _angle, CARTYPE _type) { //, std::vector<RoadTile>& _route={}
		position = _position;
		type = _type;
		angle = _angle;
		//route = _route;
		updateModel();
	}

	void move(float dt) {
		if (route.empty() || currentTargetIndex >= route.size() - 1) return;

		glm::vec3 start = route[currentTargetIndex].position;
		glm::vec3 end = route[currentTargetIndex + 1].position;

		// Move t forward by speed * dt / distance between start and end
		float segmentLength = glm::length(end - start);
		if (segmentLength == 0) return;  // Avoid divide-by-zero
		t += (speed * dt) / segmentLength;

		if (t >= 1.0f) {
			// Snap to the next point and advance the segment
			t = 0.0f;
			currentTargetIndex++;
			if (currentTargetIndex >= route.size() - 1) {
				position = end;
				return;
			}
			start = route[currentTargetIndex].position;
			end = route[currentTargetIndex + 1].position;
		}

		// Interpolate position
		position = glm::mix(start, end, t);

		// Update angle (direction of movement)
		glm::vec3 dir = glm::normalize(end - start);
		angle = glm::degrees(atan2(dir.x, dir.z));  // Rotate around Y-axis (X/Z plane)

		updateModel();
	}

	void updateModel() {
		model = glm::translate(glm::mat4(1.0f), position);
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	}
};
