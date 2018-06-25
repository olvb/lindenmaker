#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string_view>
#include <vector>

namespace lindenmaker {

struct Branch
{
    std::vector<glm::vec3> points;
    std::vector<Branch> forks;
    float radius_begin;
    float radius_end;
};

Branch sentence_to_tree(std::string_view sentence, float angle, float step_length, float radius, float length_decay, float radius_decay);
}
