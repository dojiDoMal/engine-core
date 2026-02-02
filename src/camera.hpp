#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "color.hpp"
#include "skybox.hpp"
#include "vector3.hpp"


class Camera {
private:
    ColorRGBA backgroundColor = {0.2f, 0.3f, 0.3f, 1.0f};
    Vector3 position = {0.0f, 2.0f, 2.0f};
    std::unique_ptr<Skybox> skybox;
    float fov = 45.0f;
    float nearDistance = 0.1f;
    float farDistance = 100.0f;
    float width = 800.0f;
    float height = 600.0f;

public:
    Camera() = default;
    ~Camera() = default;

    const Vector3& getPosition() const { return position; }
    void setPosition(const Vector3 &position) { this->position = position; }
    ColorRGBA& getBackgroundColor() { return backgroundColor; }
    void setBackgroundColor(const ColorRGBA &color) {
        this->backgroundColor = color;
    }
    void setFov(float fov) { this->fov = fov; }
    float getFov() const { return fov; }
    void setNearDistance(float nearDistance) { this->nearDistance = nearDistance; }
    float getNearDistance() const { return nearDistance; }
    void setFarDistance(float farDistance) { this->farDistance = farDistance; }
    float getFarDistance() const { return farDistance; }
    void setWidth(float width) { this->width = width; }
    float getWidth() const { return width; }
    void setHeight(float height) { this->height = height; }
    float getHeight() const { return height; }
    float getAspectRatio() const { return width / height; }
    void setViewRect(float width, float height) {
        setWidth(width);
        setHeight(height);
    }
    void setSkybox(std::unique_ptr<Skybox> skybox) {
        this->skybox = std::move(skybox);
    }
    Skybox *getSkybox() const { return skybox.get(); }
};

#endif // CAMERA_HPP
