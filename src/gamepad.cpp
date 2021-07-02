#include "gamepad.h"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

//-------------------------------------------------------------------------------------------------------------
// Button
//-------------------------------------------------------------------------------------------------------------

Button::Button(BYTE vKey, BYTE bScan, DWORD dwFlags) {
  cfg_.vKey = vKey;
  cfg_.bScan = bScan;
  cfg_.dwFlags = dwFlags;
}

Button::Button(ButtonConfig buttonConfig) {
  cfg_ = buttonConfig;
}

void Button::press() {
  sendInput(cfg_.vKey, cfg_.bScan, cfg_.dwFlags);
  isPressed_ = true;
}

void Button::release() {
  sendInput(cfg_.vKey, cfg_.bScan, KEYEVENTF_KEYUP | cfg_.dwFlags);
  isPressed_ = false;
}

void Button::sendInput(BYTE vKey, BYTE bScan, DWORD dwFlags) {
  INPUT ip{0};
  ip.type = INPUT_KEYBOARD;
  ip.ki.wScan = bScan;
  ip.ki.time = 0;
  ip.ki.dwExtraInfo = 0;
  ip.ki.wVk = vKey;
  ip.ki.dwFlags = dwFlags;
  SendInput(1, &ip, sizeof(INPUT));
}

//-------------------------------------------------------------------------------------------------------------
// GamePad
//-------------------------------------------------------------------------------------------------------------

GamePad::GamePad(HWND hWnd) : hWnd_(hWnd) {
  HRESULT result = 0;
  result = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8,
    (void**)&pInput_, NULL);

  if (FAILED(result))
    throw result;

  hButtonEvent_ = CreateEvent(NULL, NULL, FALSE, NULL);

  if (!hButtonEvent_)
    throw "Failed to create event";
}

GamePad::~GamePad() {
  if (hButtonEvent_)
    CloseHandle(hButtonEvent_);

  if (pGamepadDevice_) {
    pGamepadDevice_->Release();
    pGamepadDevice_ = nullptr;
  }

  if (pInput_) {
    pInput_->Release();
    pInput_ = nullptr;
  }
}

void GamePad::open(int index) {
  openIndex_ = index;
  enumCount_ = 0;

  HRESULT result = 0;
  result = pInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, _enumDeviceCallback, this, DIEDFL_ATTACHEDONLY);

  if (!pGamepadDevice_) {
    printf("No gamepad found!\n");
    throw S_FALSE;
  }

  result = pGamepadDevice_->Acquire();

  if (FAILED(result)) {
    printf("Failed to acquire gamepad!\n");
    throw result;
  }

  printf("Gamepad acquired.\n");
}

void GamePad::close() {
  HRESULT result = pGamepadDevice_->Unacquire();

  if (FAILED(result)) {
    printf("Failed unacquire gamepad!\n");
    throw result;
  }

  printf("Gamepad unacquired.\n");
}

bool GamePad::waitForButtonEvent(int timeoutMs) {
  if (WaitForSingleObject(hButtonEvent_, timeoutMs) != STATUS_WAIT_0)
    return false;

  return true;
}

DIJOYSTATE GamePad::getJoyState() const {
  DIJOYSTATE joyState;
  HRESULT result = pGamepadDevice_->GetDeviceState(sizeof(DIJOYSTATE), &joyState);

  if (FAILED(result))
    throw result;

  return joyState;
}

BOOL GamePad::_enumDeviceCallback(LPCDIDEVICEINSTANCE pLpddi, LPVOID pVref) {
  GamePad* const pThis = static_cast<GamePad*>(pVref);
  printf("%d: %s", pThis->enumCount_, pLpddi->tszInstanceName);

  if (pThis->enumCount_ == pThis->openIndex_) {
    HRESULT result{0};

    pThis->pGamepadInstance_ = pLpddi;
    printf(" [selected]\n");

    result = pThis->pInput_->CreateDevice(pLpddi->guidInstance, &pThis->pGamepadDevice_, NULL);
    if (FAILED(result)) {
      printf("Failed to create gamepad!\n");
      throw result;
    }

    printf("Gamepad created.\n");

    result = pThis->pGamepadDevice_->SetCooperativeLevel(pThis->hWnd_, DISCL_BACKGROUND | DISCL_EXCLUSIVE);

    if (FAILED(result)) {
      printf("Failed to set cooperation level!\n");
      throw result;
    }

    printf("Cooperation level set.\n");

    result = pThis->pGamepadDevice_->SetDataFormat(&c_dfDIJoystick);

    if (FAILED(result)) {
      printf("Failed to set data format!\n");
      throw result;
    }

    printf("Data format set.\n");

    pThis->pGamepadDevice_->SetEventNotification(pThis->hButtonEvent_);
    printf("Notification event set.\n");

    DIDEVCAPS caps;
    caps.dwSize = sizeof(DIDEVCAPS);
    result = pThis->pGamepadDevice_->GetCapabilities(&caps);

    if (FAILED(result)) {
      printf("Failed to get gamepad capabilities!\n");
      throw result;
    }

    printf("Gamepad capabilities:\n");
    printf("  Axes: %d\n", caps.dwAxes);
    printf("  Buttons: %d\n", caps.dwButtons);
  }
  else
    printf("\n");

  pThis->enumCount_++;

  return DIENUM_CONTINUE;
}
