#include "PCH.h"

bool ShouldClose(Window* window)
{
  return glfwWindowShouldClose(window->glfw);
}

void InitWindow(Window* window, i32 widht, i32 height, const char* tile)
{
  glfwInit();

  // hints
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window->glfw = glfwCreateWindow(widht, height, tile, 0, 0);
}

void TermWindow(Window* window)
{
  glfwDestroyWindow(window->glfw);

  glfwTerminate();
}

void PollEvents(Window* window)
{
  glfwPollEvents();
}
