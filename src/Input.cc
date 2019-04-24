//
// Created by gahwon on 4/17/19.
//

#include "Input.h"

float Input::yaw;
float Input::pitch;

float Input::zoom;
float Input::strafe;
float Input::updown;
bool Input::jump;

bool Input::enable_gravity = true;

void Input::update(GLFWwindow *window, float window_width, float window_height) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    zoom = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        zoom -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        zoom += 1.0f;

    strafe = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        strafe -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        strafe += 1.0f;

    updown = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        updown -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        updown += 1.0f;

    jump = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
}
