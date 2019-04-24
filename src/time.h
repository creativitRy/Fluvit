//
// Created by gahwon on 4/16/19.
//

#ifndef GLSL_TIME_H
#define GLSL_TIME_H


struct Time {
    static float time;
    static float delta_time;

    static void update();
};

#endif //GLSL_TIME_H
