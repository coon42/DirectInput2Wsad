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
  GamePad(HWND hWnd);
  ~GamePad();
  void open(int index);
  void close();
  bool waitForButtonEvent(int timeoutMs);

protected:
  LPDIRECTINPUTDEVICE8 pGamepadDevice_{nullptr}; // CHECKME: make private?

private:
  static BOOL _enumDeviceCallback(LPCDIDEVICEINSTANCE pLpddi, LPVOID pVref);

  const HWND hWnd_{0};
  IDirectInput8* pInput_{nullptr};
  LPCDIDEVICEINSTANCE pGamepadInstance_{nullptr};
  int openIndex_{0};
  int enumCount_{0};
  HANDLE hButtonEvent_{INVALID_HANDLE_VALUE};
};

//-------------------------------------------------------------------------------------------------------------
// DualShock2
//-------------------------------------------------------------------------------------------------------------

class DualShock2 : public GamePad {
public:
  DualShock2(HWND hWnd);

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

  State getButtonState();

private:
  static State joyState2Psx(const DIJOYSTATE& joyState);
};

//-------------------------------------------------------------------------------------------------------------
// Input
//-------------------------------------------------------------------------------------------------------------

class Input {
public:
  Input();
  ~Input();

  void run();

private:
  void createDummyWindow();
  void pressKey(WORD vKey, bool isExtendedKey = false);
  void releaseKey(WORD vKey, bool isExtendedKey = false);
  void processButtons(const DualShock2::State& psxState);
  static LRESULT CALLBACK _wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  HWND hWnd_{0};
  const HINSTANCE hInstance_{0};
  bool running_{true};
  DualShock2::State prevPsxState_{0};
};

#endif // INPUT_H