//
// Created by gahwon on 4/17/19.
//

#include <iostream>
#include "Input.h"

double Input::mouse_x;
double Input::mouse_y;

float Input::mouse_x_delta;
float Input::mouse_y_delta;

bool Input::left_click;
bool Input::right_click;

float Input::zoom;
float Input::strafe;
float Input::updown;

void Input::update(GLFWwindow *window, float window_width, float window_height) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    mouse_x_delta = x - mouse_x;
    mouse_y_delta = y - mouse_y;

    mouse_x = x;
    mouse_y = y;

    left_click = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    right_click = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

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
}
