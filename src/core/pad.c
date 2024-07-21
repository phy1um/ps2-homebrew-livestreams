#include <lua.h>
#include <p2g/pad.h>

#define DEADZONE 0.2

static int pad_lua_button_held(lua_State *l) {
  int button_id = lua_tointeger(l, 1);
  // TODO(Tom Marks): bounds check
  int v = button_held(button_id);
  // info("TEST BTN %d = %d", button_id, v);
  lua_pushboolean(l, v);
  return 1;
}

static int pad_lua_joy_value(lua_State *l) {
  int axis_id = lua_tointeger(l, 1);
  // logdbg("axis %d = %d", axis_id, joysticks[axis_id]);
  float joy_value = ((joy_axis_value(axis_id) - 127) * 1.0) / 127.0;
  if ((joy_value < 0 && joy_value > -DEADZONE) ||
      (joy_value > 0 && joy_value < DEADZONE)) {
    lua_pushinteger(l, 0);
  } else {
    lua_pushnumber(l, joy_value);
  }
  return 1;
}

#define bind_int(v, name)                                                      \
  lua_pushinteger(l, v);                                                       \
  lua_setfield(l, -2, name)
int pad_lua_init(lua_State *l) {
  if (pad_init() == -1) {
    lua_pushstring(l, "failed to init pad");
    lua_error(l);
    return 1;
  }
  lua_createtable(l, 0, 5);
  lua_pushcfunction(l, pad_lua_button_held);
  lua_setfield(l, -2, "held");
  lua_pushcfunction(l, pad_lua_joy_value);
  lua_setfield(l, -2, "axis");
  bind_int(BUTTON_X, "X");
  bind_int(BUTTON_SQUARE, "SQUARE");
  bind_int(BUTTON_TRIANGLE, "TRIANGLE");
  bind_int(BUTTON_CIRCLE, "CIRCLE");
  bind_int(DPAD_LEFT, "LEFT");
  bind_int(DPAD_RIGHT, "RIGHT");
  bind_int(DPAD_UP, "UP");
  bind_int(DPAD_DOWN, "DOWN");
  bind_int(BUTTON_SELECT, "SELECT");
  bind_int(BUTTON_START, "START");
  bind_int(BUTTON_L1, "L1");
  bind_int(BUTTON_L2, "L2");
  bind_int(BUTTON_R1, "R1");
  bind_int(BUTTON_R2, "R2");
  bind_int(AXIS_LEFT_X, "axisLeftX");
  bind_int(AXIS_LEFT_Y, "axisLeftY");
  bind_int(AXIS_RIGHT_X, "axisRightX");
  bind_int(AXIS_RIGHT_Y, "axisRightY");
  return 1;
}
