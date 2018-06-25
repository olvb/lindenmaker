#include "camera.hpp"
#include "glad.hpp"
#include "scene.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <thread>

using namespace std;
using namespace lindenmaker;

const char* WINDOW_TITLE = "Lindenmaker Demo";
const int WIDTH = 800;
const int HEIGHT = 600;
const float MOVEMENT_SPEED = 2.5f;
const float KEYBOARD_ROTATION_SPEED = 1.0f;
const float SCROLL_ZOOM_SPEED = 15.0f; // smaller => faster
const bool DONT_BURN_MY_GPU = true;

Scene* scene = nullptr;
Camera* camera = nullptr;

static float prev_frame = 0.0f;
static float scale = 1.0f;
static float x_rotation = 0.0f, y_rotation = 0.0f;

void handle_resize(int width, int height)
{
    glViewport(0, 0, width, height);
    camera->set_aspect_ratio((float) width / (float) height);
}

void handle_keyboard(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        scene->gen_tree();
        scale = 1.0f;
        x_rotation = 0.0f;
        y_rotation = 0.0f;
        prev_frame = 0.0;
        return;
    }

    float current_frame = glfwGetTime();
    float elapsed_time = current_frame - prev_frame;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        y_rotation -= elapsed_time * KEYBOARD_ROTATION_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        y_rotation += elapsed_time * KEYBOARD_ROTATION_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        x_rotation += elapsed_time * KEYBOARD_ROTATION_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        x_rotation -= elapsed_time * KEYBOARD_ROTATION_SPEED;
    }

    scene->set_tree_rotation(x_rotation, y_rotation);
    prev_frame = current_frame;
}

// void handle_cursor_movement(double x, double y)
// {
//     static float x_rotation = 0.0f, y_rotation = 0.0f;
//     static float prev_x = -1.0f, prev_y = -1.0f;

//     if (prev_x == -1.0f) {
//         assert(prev_y == -1.0f);
//         prev_x = x;
//         prev_y = y;
//         return;
//     }

//     float delta_x = x - prev_x;
//     float delta_y = y - prev_y;

//     x_rotation += delta_x * KEYBOARD_ROTATION_SPEED;
//     y_rotation -= delta_y * KEYBOARD_ROTATION_SPEED;

//     y_rotation = std::min(y_rotation, (float) M_PI / 2.0f);
//     y_rotation = std::max(y_rotation, (float) -M_PI / 2.0f);

//     scene->set_tree_rotation(x_rotation, y_rotation);

//     prev_x = x;
//     prev_y = y;
// }

void handle_scroll(double y_offset)
{
    scale += y_offset / SCROLL_ZOOM_SPEED;

    scale = std::max(scale, 0.005f);
    scale = std::min(scale, 100.0f);

    scene->set_tree_scale(scale);
}

int main(int, char**)
{
    std::srand(std::time(NULL));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    // glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
    //     if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    //         handle_cursor_movement(x, y);
    //     }
    // });
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {
        handle_resize(width, height);
    });
    glfwSetScrollCallback(window, [](GLFWwindow*, double, double y_offset) {
        handle_scroll(y_offset);
    });

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return EXIT_FAILURE;
    }

    camera = new Camera();
    scene = new Scene();
    camera->set_aspect_ratio((float) WIDTH / (float) HEIGHT);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        handle_keyboard(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene->draw(*camera);

        glfwSwapBuffers(window);

        glfwPollEvents();

        if (DONT_BURN_MY_GPU) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
