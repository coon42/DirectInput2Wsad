#include "dualshock2.h"

//-------------------------------------------------------------------------------------------------------------
// DualShock2
//-------------------------------------------------------------------------------------------------------------

DualShock2::DualShock2(HWND hWnd, const Config& config) : GamePad(hWnd),
    triangle(config.readButton("triangle")),
    circle(config.readButton("circle")),
    cross(config.readButton("cross")),
    square(config.readButton("square")),
    start(config.readButton("start")),
    select(config.readButton("select")),
    l1(config.readButton("l1")),
    l2(config.readButton("l2")),
    r1(config.readButton("r1")),
    r2(config.readButton("r2")),
    north(config.readButton("north")),
    east(config.readButton("east")),
    south(config.readButton("south")),
    west(config.readButton("west"))
{

}

void DualShock2::processButtons() {
  // TODO: is there a proper way to clear the current line on console?
  printf("\r                                                                                        \rEvent: ");

  const State state = joyState2Psx(getJoyState());

  // Presses
  if (state.select && !prevState_.select) {
    if (!prevState_.cross) {
      printf("Press Select");
      select.press();
    }
    else
      printf("Did not press select to avoid windows key shortcut!");
  }

  if (state.start && !prevState_.start) {
    printf("Press Start");
    start.press();
  }

  if (state.north && !prevState_.north) {
    printf("Press up");
    north.press();
  }

  if (state.west && !prevState_.west) {
    printf("Press left");
    west.press();
  }

  if (state.south && !prevState_.south) {
    printf("Press down");
    south.press();
  }

  if (state.east && !prevState_.east) {
    printf("Press right");
    east.press();
  }

  if (state.triangle && !prevState_.triangle) {
    printf("Press Triangle");
    triangle.press();
  }

  if (state.circle && !prevState_.circle) {
    printf("Press Circle");
    circle.press();
  }

  if (state.cross && !prevState_.cross) {
    printf("Press Cross");
    cross.press();
  }

  if (state.square && !prevState_.square) {
    printf("Press Square");
    square.press();
  }

  if (state.l1 && !prevState_.l1) {
    printf("Press L1");
    l1.press();
  }

  if (state.r1 && !prevState_.r1) {
    printf("Press R1");
    r1.press();
  }

  if (state.l2 && !prevState_.l2) {
    printf("Press L2");
    l2.press();
  }

  if (state.r2 && !prevState_.r2) {
    printf("Press R2");
    r2.press();
  }

  // Releases
  if (!state.select && prevState_.select) {
    printf("Release Select");
    select.release();
  }

  if (!state.start && prevState_.start) {
    printf("Release Start");
    start.release();
  }

  if (!state.north && prevState_.north) {
    printf("Release up");
    north.release();
  }

  if (!state.west && prevState_.west) {
    printf("Release left");
    west.release();
  }

  if (!state.south && prevState_.south) {
    printf("Release down");
    south.release();
  }

  if (!state.east && prevState_.east) {
    printf("Release right");
    east.release();
  }

  if (!state.triangle && prevState_.triangle) {
    printf("Release Triangle");
    triangle.release();
  }

  if (!state.circle && prevState_.circle) {
    printf("Release Circle");
    circle.release();
  }

  if (!state.cross && prevState_.cross) {
    printf("Release Cross");
    cross.release();
  }

  if (!state.square && prevState_.square) {
    printf("Release Square");
    square.release();
  }

  if (!state.l1 && prevState_.l1) {
    printf("Release L1");
    l1.release();
  }

  if (!state.r1 && prevState_.r1) {
    printf("Release R1");
    r1.release();
  }

  if (!state.l2 && prevState_.l2) {
    printf("Release L2");
    l2.release();
  }

  if (!state.r2 && prevState_.r2) {
    printf("Release R2");
    r2.release();
  }

  prevState_ = state;
}

DualShock2::State DualShock2::joyState2Psx(const DIJOYSTATE& joyState) {
  State state{0};
  state.triangle = joyState.rgbButtons[0];
  state.circle = joyState.rgbButtons[1];
  state.cross = joyState.rgbButtons[2];
  state.square = joyState.rgbButtons[3];
  state.start = joyState.rgbButtons[9];
  state.select = joyState.rgbButtons[8];
  state.l1 = joyState.rgbButtons[6];
  state.l2 = joyState.rgbButtons[4];
  state.r1 = joyState.rgbButtons[7];
  state.r2 = joyState.rgbButtons[5];
  state.leftStick = joyState.rgbButtons[10];
  state.rightStick = joyState.rgbButtons[11];

  switch (joyState.rgdwPOV[0]) {
    case 0:
      state.north = true;
      break;

    case 4500:
      state.north = true;
      state.east = true;
      break;

    case 9000:
      state.east = true;
      break;

    case 13500:
      state.east = true;
      state.south = true;
      break;

    case 18000:
      state.south = true;
      break;

    case 22500:
      state.south = true;
      state.west = true;
      break;

    case 27000:
      state.west = true;
      break;

    case 31500:
      state.west = true;
      state.north = true;
      break;
  }

  return state;
}
