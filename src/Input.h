//
// Created by gahwon on 4/17/19.
//

#ifndef GLSL_INPUT_H
#define GLSL_INPUT_H


#include <GLFW/glfw3.h>

struct Input {
    static float yaw;
    static float pitch;

    static float zoom;
    static float strafe;
    static float updown;
    static bool jump;

    static bool enable_gravity;

    static void update(GLFWwindow *window, float window_width, float window_height);
};


#endif //GLSL_INPUT_H
