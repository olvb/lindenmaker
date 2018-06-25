#include "object.hpp"
#include <glm/glm.hpp>
#include <string>
using std::shared_ptr;

namespace lindenmaker {

GeometryObject::GeometryObject(shared_ptr<Geometry> geometry) : geometry_(geometry) {}

void GeometryObject::draw(const ShaderProgram& program, const glm::mat4& parent_matrix) const
{
    auto model_view_matrix = parent_matrix * transform.get_matrix();
    program.set_matrix4("model_view", model_view_matrix);

    auto normal_model_view_matrix = glm::mat3(glm::transpose(glm::inverse(model_view_matrix)));
    program.set_matrix3("normal_model_view", normal_model_view_matrix);

    geometry_->draw();
}

void GeometryObject::draw_normals(const ShaderProgram& program, const glm::mat4& parent_matrix) const
{
    // auto wor_matrix = parent_matrix * transform.get_matrix();
    // program.set_matrix4("model", world_matrix);

    // auto normal_matrix = glm::mat3(glm::transpose(glm::inverse(world_matrix)));
    // program.set_matrix4("normal_model", normal_matrix);

    // auto normals_geometry = geometry_->gen_normals_mesh();
    // normals_geometry->draw();
}
}
