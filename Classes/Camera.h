#pragma once

class Camera {
protected:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 target = glm::vec3(0);
	glm::vec3 worldUp = glm::vec3(0, 1, 0);
	glm::mat4 projection;

public:
	Camera() {
		position = glm::vec3(0);
		target = glm::vec3(0);
		worldUp = glm::vec3(0);
		projection = glm::mat4(0);
	}

	/* Getters */
	glm::vec3 getPosition() {
		return position;
	}
	glm::mat4 getProjection() {
		return projection;
	}
	glm::mat4 getViewMatrix() {
		return glm::lookAt(position, target, worldUp);
	}

	/* Setters */
	void setPos(glm::vec3 position) {
		this->position = position;
	}
	void setTarget(glm::vec3 target) {
		this->target = target;
	}
	void setWorldUp(glm::vec3 worldUp) {
		this->worldUp = worldUp;
	}

	/* Methods */
	void modPos(glm::vec3 val) {
		position += val;
	}
};