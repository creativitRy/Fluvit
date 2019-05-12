//
// Created by gahwon on 4/16/19.
//

#include "time.h"
#include "fluvit/Simulation.h"
#include <GLFW/glfw3.h>

float Time::time = 0;
float Time::delta_time = 0;

float Time::fixed_time = 0;
float Time::fixed_delta_time = 0;

void Time::init() {
    time = 0;
    fixed_time = 0;
    fixed_delta_time = simulation_constants::time_step;
}

void Time::update() {
    auto curr = (float) glfwGetTime();
    delta_time = curr - time;
    time = curr;
}

bool Time::fixed_update() {
    if (fixed_time < time * simulation_constants::time_playback_speed) {
        fixed_time += Time::fixed_delta_time;
        return true;
    }
    return false;
}