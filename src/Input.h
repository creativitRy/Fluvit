//
// Created by gahwon on 4/17/19.
//

#ifndef GLSL_INPUT_H
#define GLSL_INPUT_H


#include <GLFW/glfw3.h>

struct Input {
    static double mouse_x;
    static double mouse_y;

    static float mouse_x_delta;
    static float mouse_y_delta;

    static bool left_click;
    static bool right_click;

    static float zoom;
    static float strafe;
    static float updown;

    static bool nums[10];
    static bool prev_nums[10];

    static void update(GLFWwindow *window, float window_width, float window_height);
};


#endif //GLSL_INPUT_H
