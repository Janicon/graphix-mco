#pragma once
class Light {
protected:
    glm::vec3 color;
    float intensity;
    float ambientStr;
    glm::vec3 ambientColor;
    float specStr;
    float specPhong;

public:
    Light() {
        color = glm::vec3(1, 1, 1);
        intensity = 1.f;
        ambientStr = 0.1f;
        ambientColor = color;
        specStr = 1.f;
        specPhong = 32.0f;
    }

    glm::vec3 getColor() {
        return color;
    }
    float getIntensity() {
        return intensity;
    }
    float getAmbientStr() {
        return ambientStr;
    }
    glm::vec3 getAmbientColor() {
        return ambientColor;
    }
    float getSpecStr() {
        return specStr;
    }
    float getSpecPhong() {
        return specPhong;
    }

    void setColor(glm::vec3 color) {
        this->color = color;
    }
    void setIntensity(float intensity) {
        this->intensity = intensity;
    }

    void modIntensity(float intensity) {
        this->intensity += intensity;
        if (this->intensity < 0.05)
            this->intensity = 0.05;
    }
};