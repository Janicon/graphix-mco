#pragma once
class DirectionLight : public Light {
private:
	glm::vec3 direction;

public:
	DirectionLight() {}

    DirectionLight(glm::vec3 direction, glm::vec3 color,
        float ambientStr, glm::vec3 ambientColor,
        float specStr, float specPhong) {
        this->direction = direction;
        this->color = color;
        this->ambientStr = ambientStr;
        this->ambientColor = color;
        this->specStr = specStr;
        this->specPhong = specPhong;
    }

    glm::vec3 getDirection() {
        return direction;
    }
};