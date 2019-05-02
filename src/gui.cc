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
#include "Input.h"


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
    float aspect_ = static_cast<float>(view_width_) / view_height_;
    projection_matrix_ = glm::perspective((float) (kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
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
}

void GUI::mousePosCallback(double mouse_x, double mouse_y) {
    last_x_ = current_x_;
    last_y_ = current_y_;
    current_x_ = mouse_x;
    current_y_ = window_height_ - mouse_y;
    float delta_x = current_x_ - last_x_;
    float delta_y = current_y_ - last_y_;
    if (sqrt(delta_x * delta_x + delta_y * delta_y) < 1e-15)
        return;

    if (mouse_x > view_width_)
        return;
    glm::vec3 mouse_direction = glm::normalize(glm::vec3(delta_x, delta_y, 0.0f));
    glm::vec2 mouse_start = glm::vec2(last_x_, last_y_);
    glm::vec2 mouse_end = glm::vec2(current_x_, current_y_);
    glm::uvec4 viewport = glm::uvec4(0, 0, view_width_, view_height_);

    bool drag_camera = drag_state_ && current_button_ == GLFW_MOUSE_BUTTON_RIGHT;
    bool drag_bone = drag_state_ && current_button_ == GLFW_MOUSE_BUTTON_LEFT;

    if (drag_camera) {
        glm::vec3 axis = glm::normalize(
                orientation_ *
                glm::vec3(mouse_direction.y, -mouse_direction.x, 0.0f)
        );
        orientation_ =
                glm::mat3(glm::rotate(rotation_speed_, axis) * glm::mat4(orientation_));
        tangent_ = glm::column(orientation_, 0);
        up_ = glm::column(orientation_, 1);
        look_ = glm::column(orientation_, 2);
    }
}

void GUI::mouseButtonCallback(int button, int action, int mods) {
    if (current_x_ <= view_width_) {
        drag_state_ = (action == GLFW_PRESS);
        current_button_ = button;
        return;
    }
}

void GUI::mouseScrollCallback(double dx, double dy) {
    if (current_x_ < view_width_)
        return;
}

void GUI::updateMatrices() {
    // Compute our view, and projection matrices.
    if (fps_mode_)
        center_ = eye_ + camera_distance_ * look_;
    else
        eye_ = center_ - camera_distance_ * look_;

    view_matrix_ = glm::lookAt(eye_, center_, up_);
    light_position_ = glm::vec4(eye_, 1.0f);

    aspect_ = static_cast<float>(view_width_) / view_height_;
    projection_matrix_ = glm::perspective((float) (kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
}

MatrixPointers GUI::getMatrixPointers() const {
    MatrixPointers ret{};
    ret.projection = &projection_matrix_;
    ret.view = &view_matrix_;
    return ret;
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
