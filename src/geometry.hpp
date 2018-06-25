#pragma once
#include "curve.hpp"
#include "glad.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include <vector>

namespace lindenmaker {

using VertexIndex = unsigned int;
using Face = std::tuple<VertexIndex, VertexIndex, VertexIndex>;

enum class Primitive : GLenum
{
    triangle_strip = GL_TRIANGLE_STRIP,
    triangle_fan = GL_TRIANGLE_FAN,
    triangles = GL_TRIANGLES,
    lines = GL_LINES
};

struct Vertex
{
    glm::vec3 position = glm::vec3{ 0.0f };
    glm::vec3 normal = glm::vec3{ 0.0f };
    glm::vec3 color = glm::vec3{ 0.0f };

    Vertex() = default;
    Vertex(glm::vec3 position);
    Vertex(glm::vec3 position, glm::vec3 color);
};

class Geometry
{
public:
    Geometry(const std::vector<Vertex>& vertices, const std::vector<VertexIndex>& indices, Primitive primitive);
    ~Geometry() { clear_gl(); }

    void draw() const;

private:
    std::vector<Vertex> vertices_;
    std::vector<VertexIndex> indices_;
    Primitive primitive_;
    std::vector<Face> faces_;
    GLuint vao_, vbo_, ebo_;

    void compute_faces();
    void compute_vertex_normals();
    void init_gl();
    void clear_gl();
};

std::shared_ptr<Geometry> make_cube(
    float radius = 1.0f,
    const glm::vec3& color = glm::vec3{ 1.0f });

std::shared_ptr<Geometry> make_cylinder(
    unsigned int slices_count,
    float height = 1.0f,
    float radius = 1.0f,
    const glm::vec3& color = glm::vec3{ 1.0f });

std::shared_ptr<Geometry> make_tube(
    const Curve& curve,
    unsigned int segments_count,
    unsigned int radial_segments_count,
    float radius_begin = 1.0f,
    float radius_end = 1.0f,
    bool should_draw_caps = true,
    const glm::vec3& color = glm::vec3{ 1.0f });

std::shared_ptr<Geometry> make_icosahedron(
    float radius = 1.0f,
    const glm::vec3& color = glm::vec3{ 1.0f });
}
