#include "scene.hpp"
#include "branch.hpp"
#include "curve.hpp"
#include "geometry.hpp"
#include "glad.hpp"
#include "lsystem.hpp"
#include <cstdlib>
#include <stack>
#include <vector>
using std::stack;
using std::string;
using std::vector;

#include "glm/ext.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <iostream>

namespace lindenmaker {

static float rand_float_in(float min, float max)
{
    return min + ((float) rand() / (float) RAND_MAX) * (max - min);
}

static int rand_int_in(int min, int max)
{
    return std::round(min + ((float) rand() / (float) RAND_MAX) * (max - min));
}

Scene::Scene()
{
    gen_tree();
}

void Scene::gen_tree()
{
    auto axioms = vector<string>{ "BBA", "BBBBA", "BBBBA", "BBBBBA" };
    auto axiom = axioms[rand_int_in(0, axioms.size() - 1)];
    auto lsystem = LSystem(
        axiom,
        {
            { 'A', { "[++BB[--C][++C][__C][^^C]A]/////+BBB[--C][++C][__C][^^C]A" } },
            { 'B', { "\\B", "B" } } // stochastic rule
        });

    auto derivations_count = rand_int_in(4, 7);
    auto angle = glm::radians(rand_float_in(10.0f, 22.0f));
    float step_length = rand_float_in(0.9, 1.0f);
    float radius = rand_float_in(0.2f, 0.8f);
    float radius_decay = rand_float_in(0.9f, 1.0f);
    float length_decay = rand_float_in(0.95f, 0.99f);
    unsigned int radial_segments_count = rand_int_in(4, 10);
    float leaf_scale_multiplicator = rand_float_in(0.0f, 2.0f * 6.0f / derivations_count);

    string sentence = lsystem.derive(derivations_count);
    auto tree = sentence_to_tree(sentence, angle, step_length, radius, length_decay, radius_decay);
    auto branches = stack<Branch>{};
    branches.push(tree);

    float min_y = 9999.0f, max_y = -9999.0f;

    auto brown = glm::vec3{ 54.0f / 255.0f, 43.0f / 255.0f, 20.0f / 255.0f };
    auto green = glm::vec3{ 43.0f / 255.0f, 79.0f / 255.0f, 14.0f / 255.0f };
    auto icosahedron = make_icosahedron(1.0f, green);

    auto new_tree_object = CompositeObject<GeometryObject>{};

    unsigned int branch_counter = 0;
    while (!branches.empty()) {
        auto branch = branches.top();
        branches.pop();
        branch_counter++;

        // can happen with weird l systems rules
        if (branch.points.size() < 2) {
            for (auto& fork : branch.forks) {
                branches.push(fork);
            }
            continue;
        }

        for (const auto& point : branch.points) {
            min_y = std::min(min_y, point.y);
            max_y = std::max(max_y, point.y);
        }

        auto curve = CatmullRomCurve(branch.points);

        auto tube_object = GeometryObject{ make_tube(curve, 15, radial_segments_count, branch.radius_begin, branch.radius_end, true, brown) };
        new_tree_object.add_object(tube_object);

        auto leaf_object = GeometryObject{ icosahedron };
        leaf_object.transform.translate(curve.get_point(1.0f));

        // randomized leaf size
        float min_scale = branch.radius_end * 1.5;
        float scale = min_scale + min_scale * rand_float_in(0.0f, leaf_scale_multiplicator);
        leaf_object.transform.scale(glm::vec3{ scale });

        // random leaf orientation
        float x_angle = ((float) rand() / (float) RAND_MAX) * 2.0f * M_PI;
        float y_angle = ((float) rand() / (float) RAND_MAX) * 2.0f * M_PI;
        float z_angle = ((float) rand() / (float) RAND_MAX) * 2.0f * M_PI;
        leaf_object.transform.rotate(Axis::x, x_angle);
        leaf_object.transform.rotate(Axis::y, y_angle);
        leaf_object.transform.rotate(Axis::z, z_angle);

        if (branch_counter > 0) {
            new_tree_object.add_object(leaf_object);
        }

        for (auto& fork : branch.forks) {
            branches.push(fork);
        }
    }

    // Center tree vertically
    float center_y = (max_y - min_y) / 2.0f;
    new_tree_object.transform.translate(Axis::y, -center_y);

    // Reset transforms
    set_tree_rotation(0.0f, 0.0f);
    set_tree_scale(1.0f);
    tree_object_ = new_tree_object;
}

void Scene::set_tree_rotation(float x_amount, float y_amount)
{
    tree_rotation_ = glm::mat4{ 1.0 };
    tree_rotation_ = glm::rotate(tree_rotation_, x_amount, glm::vec3{ 0.0f, 1.0f, 0.0f });
    tree_rotation_ = glm::rotate(tree_rotation_, y_amount, glm::vec3{ 1.0f, 0.0f, 0.0f });
}

void Scene::set_tree_scale(float amount)
{
    tree_scale_ = glm::scale(glm::mat4{ 1.0 }, glm::vec3{ amount });
}

void Scene::draw(const Camera& camera) const
{
    // TODO move somewhere else
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    program_.use();

    program_.set_matrix4("projection", camera.get_projection_matrix());
    program_.set_vector3("view_position", camera.get_view_position());

    const auto tree_matrix = tree_scale_ * tree_rotation_;
    program_.set_vector3("light_position", glm::vec3{ camera.get_view_matrix() * tree_matrix * glm::vec4{ light_position_, 1.0f } });

    tree_object_.draw(program_, camera.get_view_matrix() * tree_matrix);
}
}
