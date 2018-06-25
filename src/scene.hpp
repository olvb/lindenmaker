#pragma once
#include "camera.hpp"
#include "object.hpp"
#include "shader_program.hpp"
#include "transform.hpp"
#include <glm/glm.hpp>

#include "branch.hpp"

namespace lindenmaker {

class Scene
{
public:
    Scene();
    void gen_tree();
    void set_tree_rotation(float x_amount, float y_amount);
    void set_tree_scale(float amount);
    void draw(const Camera& camera) const;

private:
    // ShaderProgram program_ = ShaderProgram{ "phong.vs", "phong.fs" };
    ShaderProgram program_ = ShaderProgram{ "flat.vs", "flat.gs", "flat.fs" };
    glm::vec3 light_position_ = glm::vec3{ 5.0f, 3.0f, 0.0f };
    CompositeObject<GeometryObject> tree_object_;
    glm::mat4 tree_scale_ = glm::mat4{ 1.0 };
    glm::mat4 tree_rotation_ = glm::mat4{ 1.0 };
};
}
