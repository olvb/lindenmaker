#include "object.hpp"
#include <glm/glm.hpp>
#include <string>

namespace lindenmaker {

using std::shared_ptr;

Object::~Object() {};

GeometryObject::GeometryObject(shared_ptr<Geometry> geometry) : geometry_(geometry) {}

void GeometryObject::draw(const ShaderProgram& program, const glm::mat4& parent_matrix) const
{
    auto model_view_matrix = parent_matrix * transform.get_matrix();
    program.set_matrix4("model_view", model_view_matrix);

    auto normal_model_view_matrix = glm::mat3(glm::transpose(glm::inverse(model_view_matrix)));
    program.set_matrix3("normal_model_view", normal_model_view_matrix);

    geometry_->draw();
}
}
