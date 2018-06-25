#pragma once
#include <glm/glm.hpp>
// #include <glm/gtc/quaternion.hpp>

namespace lindenmaker {

enum class Axis
{
    x,
    y,
    z
};

class Transform
{
public:
    glm::mat4 get_matrix() const { return matrix_; }

    void scale(Axis axis, float value);
    void scale(const glm::vec3& values);
    void rotate(Axis axis, float value);
    void translate(Axis axis, float value);
    void translate(const glm::vec3& values);

private:
    glm::mat4 matrix_ = glm::mat4{ 1.0f };
};
}
