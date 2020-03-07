#ifndef INPUT_H
#define INPUT_H

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <string>

//-------------------------------------------------------------------------------------------------------------
// Config
//-------------------------------------------------------------------------------------------------------------

class Config {
public:
  Config(const std::string& configPath);

private:
  void createDefaultConfig();

  const std::string configPath_;
};

//-------------------------------------------------------------------------------------------------------------
// GamePad
//-------------------------------------------------------------------------------------------------------------

class GamePad {
public:
  GamePad();
  ~GamePad();
  void open(HWND hWnd, int index);
  void close();

private:
  static BOOL _enumDeviceCallback(LPCDIDEVICEINSTANCE pLpddi, LPVOID pVref);

  IDirectInput8* pInput_{nullptr};
  LPCDIDEVICEINSTANCE pGamepadInstance_{nullptr};
  LPDIRECTINPUTDEVICE8 pGamepadDevice_{nullptr};
  int openIndex_{0};
  int enumCount_{0};
  HWND hWnd_{0};
};

//-------------------------------------------------------------------------------------------------------------
// DualShock2
//-------------------------------------------------------------------------------------------------------------

class DualShock2 : public GamePad {
public:
  struct State { // TODO: make private
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

  static State joyState2Psx(const DIJOYSTATE& joyState); // TODO: make private

private:  
};

//-------------------------------------------------------------------------------------------------------------
// Input
//-------------------------------------------------------------------------------------------------------------

class Input {
public:
  Input();
  ~Input();

  void processOld();

private:
  void createDummyWindow();
  void enumGamepadsOld();
  void pressKey(WORD vKey, bool isExtendedKey = false);
  void releaseKey(WORD vKey, bool isExtendedKey = false);
  void processKeys();
  static BOOL _enumDeviceCallbackOld(LPCDIDEVICEINSTANCE pLpddi, LPVOID pVref);
  static LRESULT CALLBACK _wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  IDirectInput8* pInputOld_{nullptr};
  LPCDIDEVICEINSTANCE pGamepadInstanceOld_{nullptr};
  LPDIRECTINPUTDEVICE8 pGamepadDeviceOld_{nullptr};
  int enumCountOld_{0};
  HWND hWnd_{0};
  const HINSTANCE hInstance_{0};
  bool running_{true};
  DualShock2::State prevPsxState_{0};
};

#endif // INPUT_H