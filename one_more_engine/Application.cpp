
#include "PCH.h"

f32
GetTime(i64 awake)
{
  return (std::chrono::steady_clock::now().time_since_epoch().count() - awake) * 0.000000001f;
}

int
RunApplication(Application* app)
{
  InitWindow(&app->window, 800, 600, "one_more_engine");

  Input input;

  app->awakeTime = std::chrono::steady_clock::now().time_since_epoch().count();

  f32 timer = 0.f;
  f32 current = GetTime(app->awakeTime);
  f32 accumulator = 0.f;
  f32 fresh = 0.f;
  f32 delta = 0.f;

  while (!ShouldClose(&app->window))
  {
    fresh = GetTime(app->awakeTime);

    delta = fresh - current;

    if (delta > 0.25f)
    {
      delta = 0.25f;
    }

    current = fresh;

    accumulator += delta;

    UpdateInput(&input, &app->window);

    // update

    while (accumulator >= FIXED_TIME)
    {
      // fixed update

      accumulator -= FIXED_TIME;
      timer += FIXED_TIME;
    }

    // draw
    PollEvents(&app->window);
  }

  TermWindow(&app->window);

  return 0;
}