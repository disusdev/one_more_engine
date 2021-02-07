
#include "PCH.h"

i32 active_joystick = -1;
bool joystick_update = true;

void
ZeroJoysticks(Input* input)
{
  memset(input->axis, 0, sizeof(f32) * AXIS_COUNT);
  memset(input->button_states, 0, sizeof(u8) * BUTTON_COUNT);
}

void
GetActiveJoystick()
{
  active_joystick = -1;

  bool present_1 = glfwJoystickPresent(GLFW_JOYSTICK_1);
  bool present_2 = glfwJoystickPresent(GLFW_JOYSTICK_2);
  bool present_3 = glfwJoystickPresent(GLFW_JOYSTICK_3);
  bool present_4 = glfwJoystickPresent(GLFW_JOYSTICK_4);

  if (present_1 || present_2 || present_3 || present_4)
  {
    int axesCount;
    const f32* axes;

    for (int joy_num = 0; joy_num < 4; joy_num++)
    {
      axes = glfwGetJoystickAxes(joy_num, &axesCount);

      if (axesCount < 4)
      {
        continue;
      }

      active_joystick = joy_num;
      break;
    }
  }
}

void
UpdateInput(Input* input, Window* window)
{
  if (joystick_update)
  {
    joystick_update = false;
    GetActiveJoystick();

    if (active_joystick == -1)
    {
      ZeroJoysticks(input);
    }
  }

  input->hold_keys = 0x0;
  input->hold_buttons = 0x0;

  input->hold_keys = glfwGetKey(window->glfw, GLFW_KEY_W) ? input->hold_keys | Key::W : input->hold_keys;
  input->hold_keys = glfwGetKey(window->glfw, GLFW_KEY_A) ? input->hold_keys | Key::A : input->hold_keys;
  input->hold_keys = glfwGetKey(window->glfw, GLFW_KEY_S) ? input->hold_keys | Key::S : input->hold_keys;
  input->hold_keys = glfwGetKey(window->glfw, GLFW_KEY_D) ? input->hold_keys | Key::D : input->hold_keys;

  if (active_joystick >= 0)
  {
    input->p_axis = glfwGetJoystickAxes(active_joystick, &input->axis_count);
    input->p_button_states = glfwGetJoystickButtons(active_joystick, &input->button_count);

    if (input->p_axis)
    {
      memcpy(input->axis, input->p_axis, sizeof(f32) * AXIS_COUNT);
    }

    if (input->p_button_states)
    {
      memcpy(input->button_states, input->p_button_states, sizeof(f32) * AXIS_COUNT);
    }

    input->hold_buttons = input->button_states[10] ? input->hold_buttons | Button::Up : input->hold_buttons;
    input->hold_buttons = input->button_states[11] ? input->hold_buttons | Button::Right : input->hold_buttons;
    input->hold_buttons = input->button_states[12] ? input->hold_buttons | Button::Down : input->hold_buttons;
    input->hold_buttons = input->button_states[13] ? input->hold_buttons | Button::Left : input->hold_buttons;
  }

  input->down_buttons = (input->hold_buttons ^ input->prev_hold_buttons) & input->hold_buttons;
  input->up_buttons = (input->hold_buttons ^ input->prev_hold_buttons) & input->prev_hold_buttons;
  input->prev_hold_buttons = input->hold_buttons;

  input->down_keys = (input->hold_keys ^ input->prev_hold_keys) & input->hold_keys;
  input->up_keys = (input->hold_keys ^ input->prev_hold_keys) & input->prev_hold_keys;
  input->prev_hold_keys = input->hold_keys;
}

bool
GetKey(Input* input, Key key)
{
  return input->hold_keys & key;
}

bool
GetKeyDown(Input* input, Key key)
{
  return input->down_keys & key;
}

bool
GetKeyUp(Input* input, Key key)
{
  return input->up_keys & key;
}

bool
GetButton(Input* input, Button button)
{
  return input->hold_buttons & button;
}

bool
GetButtonDown(Input* input, Button button)
{
  return input->down_buttons & button;
}

bool
GetButtonUp(Input* input, Button button)
{
  return input->up_buttons & button;
}
