#include <GL/glew.h>

#include "render_pass.h"
#include "gui.h"
#include "texture_to_render.h"
#include "time.h"
#include "Floor.h"

#include <memory>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>

#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>
#include <debuggl.h>
#include "common_uniforms.h"
#include "minecraft/perlin.h"
#include "Input.h"
#include "fluvit/Terrain.h"
#include "fluvit/Camera.h"
#include "fluvit/Simulation.h"

int window_width = 1600;
int window_height = 900;
const std::string window_title = "Fluvit";

common_uniforms common_uniforms::instance;

void ErrorCallback(int error, const char *description) {
    std::cerr << "GLFW Error: " << description << "\n";
}

GLFWwindow *init_glefw() {
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwSetErrorCallback(ErrorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Disable resizing, for simplicity
    glfwWindowHint(GLFW_SAMPLES, 4); // remove when doing msaa
    auto ret = glfwCreateWindow(window_width, window_height, window_title.data(), nullptr, nullptr);
    CHECK_SUCCESS(ret != nullptr);
    glfwMakeContextCurrent(ret);
    glewExperimental = GL_TRUE;
    CHECK_SUCCESS(glewInit() == GLEW_OK);
    glGetError();  // clear GLEW's error for it
    glfwSwapInterval(1);
    const GLubyte *renderer = glGetString(GL_RENDERER);  // get renderer string
    const GLubyte *version = glGetString(GL_VERSION);    // version as a string
    std::cout << "Renderer: " << renderer << "\n";
    std::cout << "OpenGL version supported:" << version << "\n";

    return ret;
}

int main(int argc, char *argv[]) {
    if (argc > 1 && (std::string{"-h"} == argv[1] || std::string{"--help"} == argv[1])) {
        std::cout << "Usages" << std::endl;
        std::cout << "Run with no arguments to generate random 1024x1024 terrain" << std::endl;
        std::cout << "pass file path to png to load from that heightmap (only red channel used)" << std::endl;
        return 0;
    }

    // init
    Time::init();
    perlin::init();
    GLFWwindow *window = init_glefw();
    GUI gui(window);

    // entities
    Simulation sim{argc == 2 ? argv[1] : ""};
    Terrain terrain{&sim};
    Camera camera((float) window_width / window_height);
    std::function<glm::vec3()> cam_data = [&camera]() { return camera.getPos(); };
    Floor floor;
    floor.setCameraPos(cam_data);

    // shaders
    glm::vec4 light_position = glm::vec4(10.0f, 10.0f, 10.0f, 1.0f);
    MatrixPointers mats{}; // Define MatrixPointers here for lambda to capture

    // CAVEAT: DO NOT RETURN const T& in functions, which compiles but causes segfaults.
    std::function<glm::mat4()> view_data = [&mats]() { return *mats.view; };
    std::function<glm::mat4()> proj_data = [&mats]() { return *mats.projection; };
    std::function<glm::vec4()> lp_data = [&light_position]() { return light_position; };
    std::function<float()> time_data = []() { return Time::time; };
    std::function<float()> fixed_time_data = []() { return Time::fixed_time; };

    common_uniforms::instance.view = make_uniform("view", view_data);
    common_uniforms::instance.camera_position = make_uniform("camera_position", cam_data);
    common_uniforms::instance.projection = make_uniform("projection", proj_data);
    common_uniforms::instance.light_position = make_uniform("light_position", lp_data);
    common_uniforms::instance.time = make_uniform("time", time_data);
    common_uniforms::instance.fixed_time = make_uniform("fixed_time", fixed_time_data);

    // start
    sim.start();
    terrain.start();
    camera.start();
    floor.start();

    while (!glfwWindowShouldClose(window)) {
        Time::update();

        // update
        {
            Input::update(window, window_width, window_height);

            camera.update();

            while (Time::fixed_update())
                sim.update();
        }

        // Setup some basic window stuff.
        std::stringstream title;
        float cur_time = 1.0f / Time::delta_time;
        title << window_title << " FPS: "
              << std::setprecision(2)
              << std::setfill('0') << std::setw(2)
              << cur_time;
        glfwSetWindowTitle(window, title.str().data());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glfwGetFramebufferSize(window, &window_width, &window_height);
        glViewport(0, 0, window_width, window_height);

        camera.render();
        mats = camera.getMatrixPointers();

        // render
        {
            if (terrain.enabled)
                terrain.render();

            if (floor.enabled)
                floor.render();
        }

        // Poll and swap.
        glfwPollEvents();
        glfwSwapBuffers(window);

    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
