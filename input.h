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
// Button
//-------------------------------------------------------------------------------------------------------------

class Button {
public:
  Button() = default;
  Button(BYTE vKey, BYTE bScan = 0, DWORD dwFlags = 0) : vKey_(vKey) {}
  void press()                    { keybd_event(vKey_, bScan_, dwFlags_, 0); }
  void release()                  { keybd_event(vKey_, bScan_, KEYEVENTF_KEYUP | dwFlags_, 0); }

private:
  BYTE vKey_{0};
  BYTE bScan_{0};
  DWORD dwFlags_{0};
};

//-------------------------------------------------------------------------------------------------------------
// DualShock2
//-------------------------------------------------------------------------------------------------------------

class DualShock2 : public GamePad {
public:
  DualShock2(HWND hWnd);

  Button triangle;
  Button circle;
  Button cross;      // TODO: implement
  Button square;
  Button start;      // TODO: implement
  Button select;     // TODO: implement
  Button l1;         // TODO: implement
  Button l2;         // TODO: implement
  Button r1;         // TODO: implement
  Button r2;         // TODO: implement
  Button leftStick;  // TODO: implement
  Button rightStick; // TODO: implement
  Button north;      // TODO: implement
  Button east;       // TODO: implement
  Button south;      // TODO: implement
  Button west;       // TODO: implement

  // TODO: make private
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

  State getButtonState(); // TODO: make private

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
  void processButtons(DualShock2* pDualShock2, const DualShock2::State& psxState);
  static LRESULT CALLBACK _wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  HWND hWnd_{0};
  const HINSTANCE hInstance_{0};
  bool running_{true};
  DualShock2::State prevPsxState_{0};
};

#endif // INPUT_H