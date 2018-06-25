#include "camera.hpp"

namespace lindenmaker {

Camera::Camera(const glm::vec3& position) : position_(position)
{
    update_view_matrix_();
    update_projection_matrix_();
}

void Camera::dolly(float amount)
{
    position_ += front_ * amount;
    update_view_matrix_();
}

void Camera::truck(float amount)
{
    auto right = glm::normalize(glm::cross(front_, glm::vec3{ 0.0f, 1.0f, 0.0f }));
    position_ += right * amount;
    update_view_matrix_();
}

void Camera::pedestal(float amount)
{
    auto right = glm::normalize(glm::cross(front_, glm::vec3{ 0.0f, 1.0f, 0.0f }));
    auto up = glm::normalize(glm::cross(right, front_));
    position_ += up * amount;
    update_view_matrix_();
}

void Camera::tilt(float amount)
{
    pitch_ += amount;
    update_view_matrix_();
}

void Camera::pan(float amount)
{
    yaw_ += amount;
    update_view_matrix_();
}

void Camera::set_aspect_ratio(float aspect_ratio)
{
    aspect_ratio_ = aspect_ratio;
    update_projection_matrix_();
}

void Camera::update_view_matrix_()
{
    front_ = glm::vec3{
        cos(glm::radians(yaw_)) * cos(glm::radians(pitch_)),
        sin(glm::radians(pitch_)),
        sin(glm::radians(yaw_)) * cos(glm::radians(pitch_))
    };
    front_ = glm::normalize(front_);
    auto right = glm::normalize(glm::cross(front_, glm::vec3{ 0.0f, 1.0f, 0.0f }));
    auto up = glm::normalize(glm::cross(right, front_));

    view_matrix_ = glm::lookAt(position_, position_ + front_, up);
}

void Camera::update_projection_matrix_()
{
    projection_matrix_ = glm::perspective(glm::radians(45.0f), aspect_ratio_, 0.01f, 300.0f);
}
}
