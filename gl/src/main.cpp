#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdlib>

#include "test.h"

static GLFWwindow *g_window = nullptr;

void platform::drawPoint(float x, float y, float r, float g, float b, float a)
{
    glColor4f(r, g, b, a);
    glVertex2f(x, y);
}

static void mouse_callback(GLFWwindow *w, int button, int action, int)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        double x, y;
        glfwGetCursorPos(w, &x, &y);
        test::on_click(static_cast<float>(x), static_cast<float>(y));
    }
}

int main()
{
    if (!glfwInit())
        return -1;

    g_window = glfwCreateWindow(
        static_cast<int>(test::SCREEN_WIDTH),
        static_cast<int>(test::SCREEN_HEIGHT),
        "Particler",
        nullptr,
        nullptr);

    if (!g_window)
        return -1;

    glfwMakeContextCurrent(g_window);
    glfwSetMouseButtonCallback(g_window, mouse_callback);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, test::SCREEN_WIDTH, 0, test::SCREEN_HEIGHT, 0, 40);
    glMatrixMode(GL_MODELVIEW);
    glPointSize(5.0f);

    test::init();
    std::atexit(test::term);

    auto last = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(g_window))
    {
        auto now = std::chrono::steady_clock::now();
        float delta = std::chrono::duration<float>(now - last).count();
        last = now;

        test::update(delta);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_POINTS);
        test::render();
        glEnd();

        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
