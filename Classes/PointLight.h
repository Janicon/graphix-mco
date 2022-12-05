#pragma once
class PointLight : public Light {
private:
    glm::vec3 position;

public:
    PointLight() {}

    PointLight(glm::vec3 position, glm::vec3 color,
        float ambientStr, glm::vec3 ambientColor,
        float specStr, float specPhong) {
        this->position = position;
        this->color = color;
        this->ambientStr = ambientStr;
        this->ambientColor = color;
        this->specStr = specStr;
        this->specPhong = specPhong;
    }

    glm::vec3 getPos() {
        return position;
    }

    void setPos(glm::vec3 position) {
        this->position = position;
    }
};