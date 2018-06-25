#include "transform.hpp"
#include <cassert>
#include <glm/gtc/matrix_transform.hpp>

namespace lindenmaker {

glm::vec3 axis_to_vector(Axis axis)
{
    if (axis == Axis::x) {
        return glm::vec3{ 1.0f, 0.0f, 0.0f };
    }
    if (axis == Axis::y) {
        return glm::vec3{ 0.0f, 1.0f, 0.0f };
    }
    assert(axis == Axis::z);
    return glm::vec3{ 0.0f, 0.0f, 1.0f };
}

void Transform::scale(Axis axis, float value)
{
    matrix_ = glm::scale(matrix_, axis_to_vector(axis) * value);
}

void Transform::scale(const glm::vec3& values)
{
    matrix_ = glm::scale(matrix_, values);
}

void Transform::rotate(Axis axis, float value)
{
    matrix_ = glm::rotate(matrix_, value, axis_to_vector(axis));
}

void Transform::translate(Axis axis, float value)
{
    matrix_ = glm::translate(matrix_, axis_to_vector(axis) * value);
}

void Transform::translate(const glm::vec3& values)
{
    matrix_ = glm::translate(matrix_, values);
}

}
