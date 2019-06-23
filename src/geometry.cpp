#include "geometry.hpp"
#include <algorithm>
#include <cassert>
#include <ctgmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad.hpp"
#include <numeric>

namespace lindenmaker {

using std::make_shared;
using std::shared_ptr;
using std::vector;

const VertexIndex RESTART_INDEX = 65535;

Vertex::Vertex(glm::vec3 position) : position(position) {}
Vertex::Vertex(glm::vec3 position, glm::vec3 color) : position(position), color(color) {}

Geometry::Geometry(const vector<Vertex>& vertices, const vector<VertexIndex>& indices, Primitive primitive)
    : vertices_(vertices), indices_(indices), primitive_(primitive)
{
    compute_faces();
    compute_vertex_normals();
    init_gl();
}

vector<Face> compute_triangle_strip_faces(const vector<VertexIndex>& indices)
{
    auto faces = vector<Face>{};
    for (auto i = 2; i < indices.size(); i++) {
        if (indices[i - 2] == RESTART_INDEX) {
            continue;
        }
        if (indices[i - 1] == RESTART_INDEX) {
            continue;
        }
        if (indices[i] == RESTART_INDEX) {
            continue;
        }

        faces.push_back({
            indices[i - 2],
            indices[i - 1],
            indices[i] //
        });
    }
    return faces;
}

vector<Face> compute_triangle_fan_faces(const vector<VertexIndex>& indices)
{
    auto faces = vector<Face>{};
    for (unsigned int i = 0; i < indices.size(); i++) {
        if (indices[i - 1] == RESTART_INDEX) {
            continue;
        }
        assert(indices[0] != RESTART_INDEX);
        assert(indices[i - 1] != RESTART_INDEX);
        assert(indices[i] != RESTART_INDEX);

        faces.push_back({
            indices[0],
            indices[i - 1],
            indices[i] //
        });
    }
    return faces;
}

vector<Face> compute_triangles_faces(const vector<VertexIndex>& indices)
{
    auto faces = vector<Face>{};
    for (unsigned int i = 0; i < indices.size(); i += 3) {
        assert(indices[i] != RESTART_INDEX);
        assert(indices[i + 1] != RESTART_INDEX);
        assert(indices[i + 2] != RESTART_INDEX);

        faces.push_back({
            indices[i],
            indices[i + 1],
            indices[i + 2] //
        });
    }
    return faces;
}

void Geometry::compute_faces()
{
    if (primitive_ == Primitive::triangle_strip) {
        faces_ = compute_triangle_strip_faces(indices_);
    } else if (primitive_ == Primitive::triangle_fan) {
        faces_ = compute_triangle_fan_faces(indices_);
    } else {
        assert(primitive_ == Primitive::triangles);
        faces_ = compute_triangles_faces(indices_);
    }
}

float vectors_angle(const glm::vec3& vector_1, const glm::vec3& vector_2)
{
    return glm::acos(glm::dot(glm::normalize(vector_1), glm::normalize(vector_2)));
}

void Geometry::compute_vertex_normals()
{
    // TODO
    for (auto& vertex : vertices_) {
        vertex.normal = glm::vec3{ 0.0f };
    }

    bool face_is_backwards = false;
    for (auto& [index_1, index_2, index_3] : faces_) {
        auto& vertex_1 = vertices_[index_1];
        auto& vertex_2 = vertices_[index_2];
        auto& vertex_3 = vertices_[index_3];

        auto delta_2_1 = vertex_2.position - vertex_1.position;
        auto delta_1_2 = vertex_1.position - vertex_2.position;
        auto delta_3_2 = vertex_3.position - vertex_2.position;
        auto delta_2_3 = vertex_2.position - vertex_3.position;
        auto delta_3_1 = vertex_3.position - vertex_1.position;
        auto delta_1_3 = vertex_1.position - vertex_3.position;

        float angle_1 = vectors_angle(delta_2_1, delta_3_1);
        float angle_2 = vectors_angle(delta_3_2, delta_1_2);
        float angle_3 = vectors_angle(delta_1_3, delta_2_3);

        auto face_normal = glm::cross(delta_3_1, delta_2_1);
        if (primitive_ == Primitive::triangle_strip && face_is_backwards) {
            face_normal *= -1.0;
        }

        vertex_1.normal += face_normal * angle_1;
        vertex_2.normal += face_normal * angle_2;
        vertex_3.normal += face_normal * angle_3;

        face_is_backwards = !face_is_backwards;
    }

    for (auto& vertex : vertices_) {
        vertex.normal = glm::normalize(vertex.normal);
    }
}

void Geometry::init_gl()
{
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_.size(), glm::value_ptr(vertices_.front().position), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(VertexIndex) * indices_.size(), (GLuint*) indices_.data(), GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, position));
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));
    // color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, color));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Geometry::clear_gl()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

void Geometry::draw() const
{
    // TODO move
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(RESTART_INDEX);

    glBindVertexArray(vao_);
    const GLenum mode = static_cast<typename std::underlying_type<Primitive>::type>(primitive_);
    glDrawElements(mode, indices_.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

shared_ptr<Geometry> make_cube(float radius, const glm::vec3& color)
{
    auto vertices = vector<Vertex>{
        Vertex{ glm::vec3{ -1.0f, -1.0f, 1.0f } }, // bottom left front
        Vertex{ glm::vec3{ -1.0f, 1.0f, 1.0f } }, // top left front
        Vertex{ glm::vec3{ 1.0f, 1.0f, 1.0f } }, // top right front
        Vertex{ glm::vec3{ 1.0f, -1.0f, 1.0f } }, // bottom right front
        Vertex{ glm::vec3{ -1.0f, -1.0f, -1.0f } }, // bottom left back
        Vertex{ glm::vec3{ -1.0f, 1.0f, -1.0f } }, // top left back
        Vertex{ glm::vec3{ 1.0f, 1.0f, -1.0f } }, // top right back
        Vertex{ glm::vec3{ 1.0f, -1.0f, -1.0f } } // bottom right back
    };

    auto indices = vector<VertexIndex>{
        1, 0, 2, 3, // front
        6, 7, // + right
        5, 4, // + back
        RESTART_INDEX,
        7, 3, 4, 0, // bottom
        5, 1, // + left
        6, 2 // + top
    };

    for (auto& vertex : vertices) {
        vertex.position = radius * glm::normalize(vertex.position);
    }

    for (auto& vertex : vertices) {
        vertex.color = color;
    }

    return make_shared<Geometry>(vertices, indices, Primitive::triangle_strip);
}

shared_ptr<Geometry> make_cylinder(const unsigned int slices_count, const float height, const float radius, const glm::vec3& color)
{
    auto vertices = vector<Vertex>(slices_count * 2);
    const float angle_factor = 2.0f * M_PI / (float) slices_count;

    for (auto i = 0, j = 0; i < slices_count; i++) {
        float angle = (float) i * angle_factor;
        float x = std::cos(angle) * radius;
        float z = std::sin(angle) * radius;

        vertices[j++] = Vertex{ glm::vec3{ x, height / 2.0f, z } };
        vertices[j++] = Vertex{ glm::vec3{ x, -height / 2.0f, z } };
    }

    auto indices = vector<VertexIndex>(slices_count * 2);
    std::iota(indices.begin(), indices.end(), 0);
    // loop back to first vertices at top and bottom
    indices.push_back(0);
    indices.push_back(1);

    for (auto& vertex : vertices) {
        vertex.color = color;
    }

    return make_shared<Geometry>(vertices, indices, Primitive::triangle_strip);
}

// cf https://stackoverflow.com/a/33419880
shared_ptr<Geometry> make_tube(
    const Curve& curve,
    const unsigned int segments_count,
    const unsigned int radial_segments_count,
    const float radius_begin,
    const float radius_end,
    bool should_draw_caps,
    const glm::vec3& color)
{
    auto vertices = vector<Vertex>{};

    float radius = radius_begin;
    const float radius_step = (radius_end - radius_begin) / segments_count;

    for (auto i = 0; i <= segments_count; i++) {
        float x = (float) i / (float) (segments_count);
        auto point = curve.get_point(x);
        auto [_, binormal, normal] = curve.get_tbn(x);

        // generate vertices for radial segment
        auto radial_vertices = vector<Vertex>{};

        const float angle_step = 2.0f * M_PI / (float) radial_segments_count;
        float angle = 0.0;

        for (auto j = 0; j < radial_segments_count; j++) {
            float x = std::cos(angle) * radius;
            float y = std::sin(angle) * radius;
            radial_vertices.push_back(point + binormal * x + normal * y);

            angle += angle_step;
        }

        vertices.insert(vertices.end(), radial_vertices.begin(), radial_vertices.end());

        radius += radius_step;
    }

    // generate indices
    auto indices = vector<VertexIndex>{};

    // for (unsigned int i = 0; i <= segments_count; i++) {
    //     for (unsigned int j = 0; j < radial_segments_count; j++) {
    //         indices.push_back(i * radial_segments_count + j);
    //     }
    //     indices.push_back(RESTART_INDEX);
    // }

    for (auto i = 1; i <= segments_count; i++) {
        auto radial_before_begin = (i - 1) * radial_segments_count;
        auto radial_begin = i * radial_segments_count;
        for (auto j = 0; j < radial_segments_count; j++) {
            indices.push_back(radial_before_begin + j);
            indices.push_back(radial_begin + j);
        }
        // close circle by looping back to first vertex of both circles
        indices.push_back(radial_before_begin);
        indices.push_back(radial_begin);

        indices.push_back(RESTART_INDEX); // TODO useless ?
    }

    if (should_draw_caps) {
        // add vertex to center of first circle
        vertices.push_back(curve.get_point(0.0f));
        auto center_index = vertices.size() - 1;

        // draw disc
        indices.push_back(RESTART_INDEX);
        for (auto i = 0; i < radial_segments_count; i++) {
            if (i % 1 == 0) {
                indices.push_back(center_index);
            }
            indices.push_back(i);
        }
        indices.push_back(0);

        // add vertex to center of last circle
        vertices.push_back(curve.get_point(1.0f));
        center_index = vertices.size() - 1;

        // draw disc
        indices.push_back(RESTART_INDEX);
        auto radial_begin = segments_count * radial_segments_count;
        for (auto i = 0; i < radial_segments_count; i++) {
            indices.push_back(radial_begin + i);
            if (i % 1 == 0) {
                indices.push_back(center_index);
            }
        }
        indices.push_back(radial_begin);
    }

    for (auto& vertex : vertices) {
        vertex.color = color;
    }

    return make_shared<Geometry>(vertices, indices, Primitive::triangle_strip);
}

// cf https://github.com/mrdoob/three.js/blob/master/src/geometries/IcosahedronGeometry.js
shared_ptr<Geometry> make_icosahedron(float radius, const glm::vec3& color)
{
    // what is t ?
    float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
    auto vertices = vector<Vertex>{
        Vertex{ glm::vec3{ -1.0, t, 0.0f } },
        Vertex{ glm::vec3{ 1.0, t, 0.0f } },
        Vertex{ glm::vec3{ -1.0, -t, 0.0f } },
        Vertex{ glm::vec3{ 1.0, -t, 0.0f } },
        Vertex{ glm::vec3{ 0.0f, -1.0, t } },
        Vertex{ glm::vec3{ 0.0f, 1.0, t } },
        Vertex{ glm::vec3{ 0.0f, -1.0, -t } },
        Vertex{ glm::vec3{ 0.0f, 1.0, -t } },
        Vertex{ glm::vec3{ t, 0.0f, -1.0 } },
        Vertex{ glm::vec3{ t, 0.0f, 1.0 } },
        Vertex{ glm::vec3{ -t, 0.0f, -1.0 } },
        Vertex{ glm::vec3{ -t, 0.0f, 1.0 } } //
    };

    for (auto& vertex : vertices) {
        vertex.position = radius * glm::normalize(vertex.position);
    }

    auto indices = vector<VertexIndex>{
        0, 11, 5, 0, 5, 1,
        0, 1, 7, 0, 7, 10,
        0, 10, 11, 1, 5, 9,
        5, 11, 4, 11, 10, 2,
        10, 7, 6, 7, 1, 8,
        3, 9, 4, 3, 4, 2,
        3, 2, 6, 3, 6, 8,
        3, 8, 9, 4, 9, 5,
        2, 4, 11, 6, 2, 10,
        8, 6, 7, 9, 8, 1 //
    };

    for (auto& vertex : vertices) {
        vertex.color = color;
    }

    return make_shared<Geometry>(vertices, indices, Primitive::triangles);
}
}
