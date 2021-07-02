#ifndef GAMEPAD_H
#define GAMEPAD_H

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <string>

#include "config.h"

//-------------------------------------------------------------------------------------------------------------
// Button
//-------------------------------------------------------------------------------------------------------------

class Button {
public:
  Button() = default;
  Button(ButtonConfig buttonConfig);

  bool isPressed() const { return isPressed_; }
  void press();
  void release();

private:
  void sendInput(BYTE vKey, BYTE bScan = 0, DWORD dwFlags = 0);

  bool isPressed_{false};
  ButtonConfig cfg_;
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

#endif // GAMEPAD_H