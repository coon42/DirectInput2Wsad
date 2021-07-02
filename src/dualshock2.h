#ifndef DUALSHOCK2_H
#define DUALSHOCK2_H

#include "gamepad.h"

//-------------------------------------------------------------------------------------------------------------
// DualShock2
//-------------------------------------------------------------------------------------------------------------

class DualShock2 : public GamePad {
public:
  DualShock2(HWND hWnd, const Config& config);
  virtual void processButtons() final override;

private:
  struct State {
    bool triangle;
    bool circle;
    bool cross;
    bool square;
    bool start;
    bool select;
    bool l1;
    bool l2;
    bool r1;
    bool r2;
    bool leftStick;
    bool rightStick;
    bool north;
    bool east;
    bool south;
    bool west;
  };

  static State joyState2Psx(const DIJOYSTATE& joyState);

  Button triangle;
  Button circle;
  Button cross;
  Button square;
  Button start;
  Button select;
  Button l1;
  Button l2;
  Button r1;
  Button r2;
  Button leftStick;
  Button rightStick;
  Button north;
  Button east;
  Button south;
  Button west;

  State prevState_{0};
};

#endif // DUALSHOCK2_H
