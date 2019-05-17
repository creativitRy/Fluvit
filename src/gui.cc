#include <GL/glew.h>
#include "gui.h"
#include <jpegio.h>
#include <iostream>
#include <algorithm>
#include <debuggl.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <unistd.h>
#include <sys/wait.h>
#include "time.h"


GUI::GUI(GLFWwindow *window, int view_width, int view_height, int preview_height)
        : window_(window), preview_height_(preview_height) {
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetCursorPosCallback(window_, MousePosCallback);
    glfwSetMouseButtonCallback(window_, MouseButtonCallback);
    glfwSetScrollCallback(window_, MouseScrollCallback);

    glfwGetWindowSize(window_, &window_width_, &window_height_);
    if (view_width < 0 || view_height < 0) {
        view_width_ = window_width_;
        view_height_ = window_height_;
    } else {
        view_width_ = view_width;
        view_height_ = view_height;
    }
}

GUI::~GUI() {
}

void GUI::keyCallback(int key, int scancode, int action, int mods) {
#if 0
    if (action != 2)
        std::cerr << "Key: " << key << " action: " << action << std::endl;
#endif
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, GL_TRUE);
        return;
    }
    if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
        // save out a screenshot using SaveJPEG
        uint8_t pixels[3 * window_width_ * window_height_];
        glReadPixels(0, 0, window_width_, window_height_, GL_RGB, GL_UNSIGNED_BYTE, pixels);

        std::cout << "Encoding and saving to file 'capture.jpg'" << std::endl;
        SaveJPEG("capture.jpg", window_width_, window_height_, pixels);
        return;
    }
    if (key == GLFW_KEY_P && action == GLFW_RELEASE) {
        Time::playing = !Time::playing;
    }
}

void GUI::mousePosCallback(double mouse_x, double mouse_y) {
    last_x_ = current_x_;
    last_y_ = current_y_;
    current_x_ = mouse_x;
    current_y_ = window_height_ - mouse_y;
}

void GUI::mouseButtonCallback(int button, int action, int mods) {
}

void GUI::mouseScrollCallback(double dx, double dy) {
    if (current_x_ < view_width_)
        return;
}

// Delegate to the actual GUI object.
void GUI::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    GUI *gui = (GUI *) glfwGetWindowUserPointer(window);
    gui->keyCallback(key, scancode, action, mods);
}

void GUI::MousePosCallback(GLFWwindow *window, double mouse_x, double mouse_y) {
    GUI *gui = (GUI *) glfwGetWindowUserPointer(window);
    gui->mousePosCallback(mouse_x, mouse_y);
}

void GUI::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    GUI *gui = (GUI *) glfwGetWindowUserPointer(window);
    gui->mouseButtonCallback(button, action, mods);
}

void GUI::MouseScrollCallback(GLFWwindow *window, double dx, double dy) {
    GUI *gui = (GUI *) glfwGetWindowUserPointer(window);
    gui->mouseScrollCallback(dx, dy);
}
