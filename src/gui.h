#ifndef SKINNING_GUI_H
#define SKINNING_GUI_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

/*
 * Hint: call glUniformMatrix4fv on thest pointers
 */
struct MatrixPointers {
    const glm::mat4 *projection, *view;
};

class GUI {
public:
    GUI(GLFWwindow *, int view_width = -1, int view_height = -1, int preview_height = -1);

    ~GUI();

    void keyCallback(int key, int scancode, int action, int mods);

    void mousePosCallback(double mouse_x, double mouse_y);

    void mouseButtonCallback(int button, int action, int mods);

    void mouseScrollCallback(double dx, double dy);

private:
    GLFWwindow *window_;

    int window_width_, window_height_;
    int view_width_, view_height_;
    int preview_height_;

    float last_x_ = 0.0f, last_y_ = 0.0f, current_x_ = 0.0f, current_y_ = 0.0f;

    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void MousePosCallback(GLFWwindow *window, double mouse_x, double mouse_y);

    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

    static void MouseScrollCallback(GLFWwindow *window, double dx, double dy);
};

#endif
