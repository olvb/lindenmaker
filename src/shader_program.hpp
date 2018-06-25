#pragma once
#include "glad.hpp"
#include <glm/glm.hpp>
#include <string_view>

namespace lindenmaker {

class ShaderProgram
{
public:
    ShaderProgram(std::string_view vertex_filename, std::string_view fragment_filename);
    ShaderProgram(std::string_view vertex_filename, std::string_view geometry_filename, std::string_view fragment_filename);

    void use() const { glUseProgram(id_); }

    void set_bool(std::string name, bool value) const;
    void set_int(std::string name, int value) const;
    void set_float(std::string name, float value) const;
    void set_vector3(std::string name, const glm::vec3& value) const;
    void set_matrix3(std::string name, const glm::mat3& value) const;
    void set_matrix4(std::string name, const glm::mat4& value) const;

private:
    GLuint id_;
};
}
