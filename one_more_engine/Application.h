#pragma once

const f32 FIXED_TIME = 1.0f / 60.0f;

struct 
Application
{
  Window window;
  i64 awakeTime;
};

int
RunApplication(Application* app);