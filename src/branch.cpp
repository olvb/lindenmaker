#include "branch.hpp"
#include <cassert>
#include <stdexcept>
#include <string>

namespace lindenmaker {

using std::string;
using std::string_view;
using std::vector;

const auto DIRECTION = glm::vec3{ 0.0f, 1.0f, 0.0f };

struct Turtle
{
    glm::vec3 position = glm::vec3{ 0.0f };
    glm::quat orientation = glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f };

    float angle;
    float step_length;
    float radius;
    float length_decay;
    float radius_decay;

    Turtle(float angle, float step_length, float radius, float length_decay, float radius_decay)
        : angle(angle),
          step_length(step_length),
          radius(radius),
          length_decay(length_decay),
          radius_decay(radius_decay)
    {
    }

    void step_foward()
    {
        position += orientation * DIRECTION * step_length;
    }

    void decay()
    {
        step_length *= length_decay;
        radius *= radius_decay;
    }

    void plus_yaw()
    {
        orientation *= glm::quat(glm::vec3{ 0.0f, 0.0f, angle });
    }

    void minus_yaw()
    {
        orientation *= glm::quat(glm::vec3{ 0.0f, 0.0f, -angle });
    }

    void plus_roll()
    {
        orientation *= glm::quat(glm::vec3{ 0.0f, angle, 0.0f });
    }

    void minus_roll()
    {
        orientation *= glm::quat(glm::vec3{ 0.0f, -angle, 0.0f });
    }

    void plus_pitch()
    {
        orientation *= glm::quat(glm::vec3{ angle, 0.0f, 0.0f });
    }

    void minus_pitch()
    {
        orientation *= glm::quat(glm::vec3{ -angle, 0.0f, 0.0f });
    }
};

Branch do_the_turtle(string_view& sentence, Turtle turtle)
{
    Branch branch;

    // push starting point
    branch.points.push_back(turtle.position);
    branch.radius_begin = turtle.radius;

    while (sentence.size() > 0) {
        const char symbol = sentence.front();
        // std::cout << symbol << std::endl;
        sentence.remove_prefix(1);

        // forward alpha char
        if (symbol >= 'A' && symbol <= 'Z') {
            turtle.step_foward();
            turtle.decay();
            continue;
        }

        // stack char
        if (symbol == '[') {
            auto branch_turtle = turtle;
            branch.forks.push_back(do_the_turtle(sentence, branch_turtle));
            continue;
        }
        if (symbol == ']') {
            break;
        }

        // rotation char, push current point
        if (turtle.position != branch.points.back()) {
            branch.points.push_back(turtle.position);
        }

        // yaw
        if (symbol == '+') {
            turtle.plus_yaw();
            continue;
        }
        if (symbol == '-') {
            turtle.minus_yaw();
            continue;
        }

        // roll
        if (symbol == '/') {
            turtle.plus_roll();
            continue;
        }
        if (symbol == '\\') {
            turtle.minus_roll();
            continue;
        }

        // pitch
        if (symbol == '^') {
            turtle.plus_pitch();
            continue;
        }
        if (symbol == '_') {
            turtle.minus_pitch();
            continue;
        }

        throw std::runtime_error(string{ "Unknown symbol: " } + symbol);
    }

    if (turtle.position != branch.points.back()) {
        branch.points.push_back(turtle.position);
        branch.radius_end = turtle.radius;
    }

    // assert(branch.points.size() > 1);
    return branch;
}

Branch sentence_to_tree(std::string_view sentence, float angle, float step_length, float radius, float length_decay, float radius_decay)
{
    auto turtle = Turtle{ angle, step_length, radius, length_decay, radius_decay };
    return do_the_turtle(sentence, turtle);
}
}
