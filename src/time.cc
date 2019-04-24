//
// Created by gahwon on 4/16/19.
//

#include "time.h"
#include <GLFW/glfw3.h>

float Time::time = 0;
float Time::delta_time = 0;

void Time::update() {
    auto curr = (float) glfwGetTime();
    delta_time = curr - time;
    time = curr;
}
