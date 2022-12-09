#pragma once
class PointLight : public Light {
private:
    glm::vec3 position;
    float linear;
    float quadratic;

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
        linear = 0.0014f;
        quadratic = 0.000007f;
    }

    glm::vec3 getPos() {
        return position;
    }

    float getLinear() {
        return linear;
    }

    float getQuadratic() {
        return quadratic;
    }

    void setPos(glm::vec3 position) {
        this->position = position;
    }

    void setAttenuation(float linear, float quadratic) {
        this->linear = linear;
        this->quadratic = quadratic;
    }
};