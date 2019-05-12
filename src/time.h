//
// Created by gahwon on 4/16/19.
//

#ifndef GLSL_TIME_H
#define GLSL_TIME_H


struct Time {
    static float time;
    static float delta_time;

    static float fixed_time;
    static float fixed_delta_time;

    static void init();
    static void update();
    static bool fixed_update();
};

#endif //GLSL_TIME_H
