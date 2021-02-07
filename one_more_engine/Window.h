#pragma once

struct 
Window
{
  GLFWwindow* glfw;
  // renderer
  // camera
};

bool
ShouldClose(Window* window);

void
InitWindow(Window* window, i32 widht, i32 height, const char* tile);

void
TermWindow(Window* window);

void
PollEvents(Window* window);

