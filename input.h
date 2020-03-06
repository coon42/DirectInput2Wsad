#ifndef INPUT_H
#define INPUT_H

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

//-------------------------------------------------------------------------------------------------------------
// Input
//-------------------------------------------------------------------------------------------------------------

class Input {
public:
  Input();
  ~Input();

  void process();

private:
  struct DualShock2State {
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

  DualShock2State joyState2Psx(const DIJOYSTATE& joyState);
  void createDummyWindow();
  void enumGamepads();
  void pressKey(WORD vKey, bool isExtendedKey = false);
  void releaseKey(WORD vKey, bool isExtendedKey = false);
  void processKeys();
  static BOOL _enumDeviceCallback(LPCDIDEVICEINSTANCE pLpddi, LPVOID pVref);
  static LRESULT CALLBACK _wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  IDirectInput8* pInput_{nullptr};
  LPCDIDEVICEINSTANCE pGamepadInstance_{nullptr};
  LPDIRECTINPUTDEVICE8 pGamepadDevice_{nullptr};
  int enumCount_{0};
  HWND hWnd_{0};
  const HINSTANCE hInstance_{0};
  bool running_{true};
  DualShock2State prevPsxState_{0};
};

#endif // INPUT_H