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
  virtual void processButtons() = 0;

protected:
  DIJOYSTATE getJoyState() const;

private:
  static BOOL _enumDeviceCallback(LPCDIDEVICEINSTANCE pLpddi, LPVOID pVref);

  const HWND hWnd_{0};
  IDirectInput8* pInput_{nullptr};
  LPDIRECTINPUTDEVICE8 pGamepadDevice_{nullptr};
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
  Button(BYTE vKey, BYTE bScan = 0, DWORD dwFlags = 0) : vKey_(vKey), bScan_(bScan), dwFlags_(dwFlags) {}
  bool isPressed() const { return isPressed_; }

  void press() {
    sendInput(vKey_, bScan_, dwFlags_);
    isPressed_ = true;
  }

  void release() {
    sendInput(vKey_, bScan_, KEYEVENTF_KEYUP | dwFlags_);
    isPressed_ = false;
  }

private:
  void sendInput(BYTE vKey, BYTE bScan = 0, DWORD dwFlags = 0) {
    INPUT ip{0};
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = bScan;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.wVk = vKey;
    ip.ki.dwFlags = dwFlags;
    SendInput(1, &ip, sizeof(INPUT));
  }

  bool isPressed_{false};
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

  static State joyState2Psx(const DIJOYSTATE& joyState);
  DualShock2::State prevState_{0};
};

//-------------------------------------------------------------------------------------------------------------
// Application
//-------------------------------------------------------------------------------------------------------------

class Application {
public:
  Application();
  ~Application();

  void run();

private:
  void createDummyWindow();
  void pressKey(WORD vKey, bool isExtendedKey = false);
  void releaseKey(WORD vKey, bool isExtendedKey = false);
  static LRESULT CALLBACK _wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  HWND hWnd_{0};
  const HINSTANCE hInstance_{0};
  bool running_{true};
};

#endif // INPUT_H