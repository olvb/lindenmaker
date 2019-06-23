#include "shader_program.hpp"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace lindenmaker {

using std::string;
using std::string_view;
using std::vector;

const auto SHADER_DIR = string { "shaders/" };

string read_shader_source(string_view filename)
{
    string file_path = SHADER_DIR + string{ filename };
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader " + file_path);
    }

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compile_shader(string_view filename, GLenum type)
{
    GLuint shader = glCreateShader(type);
    string source = read_shader_source(filename);
    const GLchar* source_c_str = source.c_str();
    glShaderSource(shader, 1, &source_c_str, NULL);
    glCompileShader(shader);

    GLint is_compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled) {
        char info_log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, info_log);
        throw std::runtime_error("Failed to compile shader " + string{ filename } + ":\n" + string{ info_log });
    }

    return shader;
}

GLuint link_program(vector<GLuint> shaders)
{
    const GLuint program = glCreateProgram();
    for (const auto shader : shaders) {
        glAttachShader(program, shader);
    }
    glLinkProgram(program);

    GLint is_linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if (!is_linked) {
        char info_log[1024];
        glGetShaderInfoLog(program, 1024, nullptr, info_log);
        throw std::runtime_error("Failed to link shader:\n" + string{ info_log });
    }

    return program;
}

ShaderProgram::ShaderProgram(string_view vertex_filename, string_view fragment_filename)
{
    const GLuint vertex_shader = compile_shader(vertex_filename, GL_VERTEX_SHADER);
    const GLuint fragment_shader = compile_shader(fragment_filename, GL_FRAGMENT_SHADER);
    id_ = link_program({ vertex_shader, fragment_shader });
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

ShaderProgram::ShaderProgram(string_view vertex_filename, string_view geometry_filename, string_view fragment_filename)
{
    const GLuint vertex_shader = compile_shader(vertex_filename, GL_VERTEX_SHADER);
    const GLuint geometry_shader = compile_shader(geometry_filename, GL_GEOMETRY_SHADER);
    const GLuint fragment_shader = compile_shader(fragment_filename, GL_FRAGMENT_SHADER);
    id_ = link_program({ vertex_shader, geometry_shader, fragment_shader });
    glDeleteShader(vertex_shader);
    glDeleteShader(geometry_shader);
    glDeleteShader(fragment_shader);
}

void ShaderProgram::ShaderProgram::set_bool(const string& name, bool value) const
{
    const GLuint location = glGetUniformLocation(id_, name.c_str());
    glUniform1i(location, (int) value);
}

void ShaderProgram::set_int(const string& name, int value) const
{
    const GLuint location = glGetUniformLocation(id_, name.c_str());
    glUniform1i(location, value);
}

void ShaderProgram::set_float(const string& name, float value) const
{
    const GLuint location = glGetUniformLocation(id_, name.c_str());
    glUniform1f(location, value);
}

void ShaderProgram::set_vector3(const string& name, const glm::vec3& value) const
{
    const GLuint location = glGetUniformLocation(id_, name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void ShaderProgram::set_matrix3(const string& name, const glm::mat3& value) const
{
    const GLuint location = glGetUniformLocation(id_, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::set_matrix4(const string& name, const glm::mat4& value) const
{
    const GLuint location = glGetUniformLocation(id_, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
}
