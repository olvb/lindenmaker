#include "curve.hpp"
#include <cassert>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace lindenmaker {

using std::tuple;
using std::vector;

const float EPSILON = 0.001f;

// Abstract curve

// cf https://stackoverflow.com/a/33419880
tuple<glm::vec3, glm::vec3, glm::vec3> Curve::get_tbn(const float x) const
{
    float x1 = std::max(x - EPSILON, 0.0f);
    float x2 = std::min(x + EPSILON, 1.0f);

    auto point_1 = get_point(x1);
    auto point_2 = get_point(x2);

    auto tangent = point_2 - point_1;
    assert(glm::length(tangent) != 0.0f);
    // DO NOT NORMALIZE TANGENT !!

    auto binormal = glm::cross(tangent, point_2 + point_1);
    if (glm::length(binormal) != 0.0f) {
        // abs: avoid brutal direction change when sign changes
        // TODO not working
        binormal = glm::normalize(glm::abs(binormal));
        //binormal = glm::normalize(binormal);
    } else {
        // avoid nan by choosing arbitrary binormal
        // TODO is this troublesome?
        if (tangent.x != 0.0) {
            binormal.y = 1.0;
        } else if (tangent.y != 0.0) {
            binormal.z = 1.0;
        } else {
            assert(tangent.z != 0.0);
            binormal.x = 1.0;
        }
    }

    auto normal = glm::cross(binormal, tangent);
    assert(glm::length(normal) != 0.0f);
    // abs: avoid brutal direction change when sign changes
    // TODO not working
    //normal = glm::normalize(glm::abs(normal));
    normal = glm::normalize(normal);
    return { tangent, binormal, normal };
}

// Circle curve

CircleCurve::CircleCurve(float radius) : radius_(radius) {}

glm::vec3 CircleCurve::get_point(const float x) const
{
    float angle = 2.0f * M_PI * x;
    return glm::vec3{ radius_ * std::cos(angle), radius_ * std::sin(angle), 0.0f };
}

// CatmullRomCurve

CatmullRomCurve::CatmullRomCurve(const vector<glm::vec3>& points) : points_(points)
{
    assert(points_.size() > 1);
}

// https://stackoverflow.com/a/23980479
glm::vec3 centripedal_catmull_rom(float x, const glm::vec3& point_0, const glm::vec3& point_1, const glm::vec3& point_2, const glm::vec3& point_3)
{
    // 0.25 => 0.5 * 0.5 (centripetal)
    float delta_0 = std::pow(glm::length(point_1 - point_0), 0.25f);
    float delta_1 = std::pow(glm::length(point_2 - point_1), 0.25f);
    float delta_2 = std::pow(glm::length(point_3 - point_2), 0.25f);

    // delta can be == 0 for (nearly) identical consecutive points
    if (delta_0 < EPSILON) {
        delta_0 = 1.0f;
    }
    if (delta_1 < EPSILON) {
        delta_1 = delta_0;
    }
    if (delta_2 < EPSILON) {
        delta_2 = delta_1;
    }

    assert(delta_0 != 0.0);
    assert(delta_1 != 0.0);
    assert(delta_2 != 0.0);

    auto tangeant_1 = ((point_1 - point_0) / delta_0) - ((point_2 - point_0) / (delta_0 + delta_1)) + ((point_2 - point_1) / delta_1);
    auto tangeant_2 = ((point_2 - point_1) / delta_1) - ((point_3 - point_1) / (delta_1 + delta_2)) + ((point_3 - point_2) / delta_2);

    tangeant_1 *= delta_1;
    tangeant_2 *= delta_1;

    const auto coef_0 = point_1;
    const auto coef_1 = tangeant_1;
    const auto coef_2 = -3.0f * point_1 + 3.0f * point_2 - 2.0f * tangeant_1 - tangeant_2;
    const auto coef_3 = 2.0f * point_1 - 2.0f * point_2 + tangeant_1 + tangeant_2;

    const float x_square = x * x;
    const float x_cube = x_square * x;

    return coef_0 + coef_1 * x + coef_2 * x_square + coef_3 * x_cube;
}

glm::vec3 CatmullRomCurve::get_point(float x) const
{
    assert(x >= 0.0f && x <= 1.0f);
    // x = std::max(x, 0.0f);
    // x = std::min(x, 1.0f);

    float float_index = (float) (points_.size() - 1) * x;
    unsigned int index_1 = std::floor(float_index);
    unsigned int index_2 = std::ceil(float_index);

    // why ?
    float weight = (float_index - (float) index_1);
    if (weight == 0.0f && index_1 == points_.size() - 1) {
        index_1 = points_.size() - 2;
        weight = 1.0f;
    }

    const auto point_1 = points_[index_1];
    const auto point_2 = points_[index_2];

    glm::vec3 point_0, point_3;
    if (index_1 > 0) {
        point_0 = points_[index_1 - 1];
    } else {
        // extrapolate point before first
        // cf. https://github.com/mrdoob/three.js/blob/master/src/extras/curves/CatmullRomCurve3.js
        const auto first = points_[0];
        const auto second = points_[1];
        const auto slope = second - first;
        point_0 = first - slope;
    }

    if (index_2 < points_.size() - 1) {
        point_3 = points_[index_2 + 1];
    } else {
        // extrapolate point after last
        // cf. https://github.com/mrdoob/three.js/blob/master/src/extras/curves/CatmullRomCurve3.js
        const auto last = points_.back();
        const auto last_but_one = points_.end()[-2];
        const auto slope = last - last_but_one;
        point_3 = last + slope;
    }

    return centripedal_catmull_rom(weight, point_0, point_1, point_2, point_3);
}
}
