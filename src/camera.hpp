#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace lindenmaker {

class Camera
{
public:
    Camera(const glm::vec3& position = glm::vec3{ 0.0f, 1.0f, 28.0f });
    glm::mat4 get_view_matrix() const { return view_matrix_; }
    glm::vec3 get_view_position() const { return position_; }
    glm::mat4 get_projection_matrix() const { return projection_matrix_; }

    void truck(float amount);
    void dolly(float amount);
    void pedestal(float amount);
    void tilt(float amount);
    void pan(float amount);

    void set_aspect_ratio(float aspect_ratio);

private:
    glm::vec3 position_;
    glm::vec3 front_ = glm::vec3{ 0.0f, 0.0f, -1.0f };
    float yaw_ = -90.0f;
    float pitch_ = 0.0f;
    float aspect_ratio_ = 1.0f;

    glm::mat4 view_matrix_ = glm::mat4{ 1.0f };
    glm::mat4 projection_matrix_ = glm::mat4{ 1.0f };

    void update_view_matrix_();
    void update_projection_matrix_();
};
}
