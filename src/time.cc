//
// Created by gahwon on 4/16/19.
//

#include "time.h"
#include "fluvit/Simulation.h"
#include <GLFW/glfw3.h>

float Time::unscaled_time = 0;
float Time::unscaled_delta_time = 0;

float Time::time = 0;
float Time::delta_time = 0;

float Time::fixed_time = 0;
float Time::fixed_delta_time = 0;

bool Time::playing = false;

void Time::init() {
    unscaled_time = 0;
    unscaled_delta_time = 0;
    time = 0;
    fixed_time = 0;
    fixed_delta_time = simulation_constants::time_step;
}

void Time::update() {
    auto curr = (float) glfwGetTime();
    unscaled_delta_time = curr - unscaled_time;
    unscaled_time = curr;

    if (playing) {
        delta_time = unscaled_delta_time;
        time += delta_time;
    } else {
        delta_time = 0.0f;
    }
}

bool Time::fixed_update() {
    if (!playing)
        return false;

    if (fixed_time < time * simulation_constants::time_playback_speed) {
        fixed_time += Time::fixed_delta_time;
        return true;
    }
    return false;
}