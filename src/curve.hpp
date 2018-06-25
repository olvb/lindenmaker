#pragma once
#include <glm/glm.hpp>
#include <tuple>
#include <vector>

namespace lindenmaker {

class Curve
{
public:
    virtual glm::vec3 get_point(float x) const = 0;
    virtual std::tuple<glm::vec3, glm::vec3, glm::vec3> get_tbn(float x) const;
};

class CircleCurve : public Curve
{
public:
    CircleCurve(float radius = 1.0f);
    glm::vec3 get_point(float x) const final;

private:
    float radius_;
};

class CatmullRomCurve : public Curve
{
public:
    CatmullRomCurve(const std::vector<glm::vec3>& points);
    glm::vec3 get_point(float x) const final;

private:
    std::vector<glm::vec3> points_;
};
}
