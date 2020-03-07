#include <stdio.h>
#include <conio.h>

#include "input.h"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

//-------------------------------------------------------------------------------------------------------------
// Config
//-------------------------------------------------------------------------------------------------------------

Config::Config(const std::string& configPath) : configPath_(configPath) {
  createDefaultConfig(); // TODO: only create default config when config file does not already exist.
}

void Config::createDefaultConfig() {
  // TODO: implement
}

//-------------------------------------------------------------------------------------------------------------
// GamePad
//-------------------------------------------------------------------------------------------------------------

GamePad::GamePad() {
  HRESULT result = 0;
  result = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8,
    (void**)&pInput_, NULL);

  if (FAILED(result))
    throw result;
}

GamePad::~GamePad() {
  if (pGamepadDevice_) {
    pGamepadDevice_->Release();
    pGamepadDevice_ = nullptr;
  }

  if (pInput_) {
    pInput_->Release();
    pInput_ = nullptr;
  }
}

void GamePad::open(HWND hWnd, int index) {
  hWnd_ = hWnd;
  openIndex_ = index; // TODO: make configurable over ini file:
  enumCount_ = 0;

  HRESULT result = 0;
  result = pInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, _enumDeviceCallback, this, DIEDFL_ATTACHEDONLY);

  // TODO: what happens if gamepad was not found or could not be opened?
}

void GamePad::close() {

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

//-------------------------------------------------------------------------------------------------------------
// DualShock2
//-------------------------------------------------------------------------------------------------------------

DualShock2::State DualShock2::joyState2Psx(const DIJOYSTATE& joyState) {
  DualShock2::State state{0};
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

//-------------------------------------------------------------------------------------------------------------
// Input
//-------------------------------------------------------------------------------------------------------------

Input::Input() : hInstance_(GetModuleHandle(NULL)) {
  createDummyWindow();

  HRESULT result = 0;
  result = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8,
    (void**)&pInputOld_, NULL);

  if (FAILED(result))
    throw result;

  enumGamepadsOld();
}

Input::~Input() {
  if (pGamepadDeviceOld_) {
    pGamepadDeviceOld_->Release();
    pGamepadDeviceOld_ = nullptr;
  }

  if (pInputOld_) {
    pInputOld_->Release();
    pInputOld_ = nullptr;
  }

  DestroyWindow(hWnd_);
  hWnd_ = 0;
}

void Input::pressKey(WORD vKey, bool isExtendedKey) {
  INPUT ip{0};
  ip.type = INPUT_KEYBOARD;
  ip.ki.wScan = 0;
  ip.ki.time = 0;
  ip.ki.dwExtraInfo = 0;
  ip.ki.wVk = vKey;
  ip.ki.dwFlags = 0 | (isExtendedKey ? KEYEVENTF_EXTENDEDKEY : 0); // 0 for key press
  SendInput(1, &ip, sizeof(INPUT));
}

void Input::releaseKey(WORD vKey, bool isExtendedKey) {
  INPUT ip{0};
  ip.type = INPUT_KEYBOARD;
  ip.ki.wScan = 0;
  ip.ki.time = 0;
  ip.ki.dwExtraInfo = 0;
  ip.ki.wVk = vKey;
  ip.ki.dwFlags = KEYEVENTF_KEYUP | (isExtendedKey ? KEYEVENTF_EXTENDEDKEY : 0);
  SendInput(1, &ip, sizeof(INPUT));
}

void Input::createDummyWindow() {
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = _wndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance_;
  wcex.hIcon = 0;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = "Dummy";
  wcex.hIconSm = 0;
  RegisterClassEx(&wcex);  

  hWnd_ = CreateWindow(wcex.lpszClassName, "dummy", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
      0, 100, 100, nullptr, nullptr, hInstance_, nullptr);

  if (!hWnd_)
    throw false;

  ShowWindow(hWnd_, SW_SHOW);
  UpdateWindow(hWnd_);
}

void Input::enumGamepadsOld() {
  enumCountOld_ = 0;

  HRESULT result = 0;
  result = pInputOld_->EnumDevices(DI8DEVCLASS_GAMECTRL, _enumDeviceCallbackOld, this, DIEDFL_ATTACHEDONLY);
}

void Input::processKeys() {
  DIJOYSTATE joyState;
  pGamepadDeviceOld_->GetDeviceState(sizeof(DIJOYSTATE), &joyState);

  const DualShock2::State psxState = DualShock2::joyState2Psx(joyState);
  printf("event!\n");

  // Presses
  if (psxState.north && !prevPsxState_.north) {
    printf("Press up");
    pressKey(VK_UP);
  }

  if (psxState.west && !prevPsxState_.west) {
    printf("Press left");
    pressKey(VK_LEFT);
  }

  if (psxState.south && !prevPsxState_.south) {
    printf("Press down");
    pressKey(VK_DOWN);
  }

  if (psxState.east && !prevPsxState_.east) {
    printf("Press right");
    pressKey(VK_RIGHT);
  }

  if (psxState.triangle && !prevPsxState_.triangle) {
    printf("Press Triangle");
    keybd_event(VK_SPACE, 0, 0, 0);
  }

  if (psxState.circle && !prevPsxState_.circle) {
    printf("Press Circle");
    keybd_event(VK_END, 0x4f, KEYEVENTF_EXTENDEDKEY, 0);
  }

  if (psxState.cross && !prevPsxState_.cross) {
    printf("Press Cross");
    keybd_event(VK_RCONTROL, 0, 0, 0);
  }

  if (psxState.square && !prevPsxState_.square) {
    printf("Press Square");
    keybd_event(VK_ADD, 0, 0, 0);
  }

  if (psxState.l1 && !prevPsxState_.l1) {
    printf("Press L1");
    keybd_event(VK_NUMPAD0, 0x52, 0, 0);
  }

  if (psxState.r1 && !prevPsxState_.r1) {
    printf("Press R1");
    keybd_event(VK_NUMPAD1, 0x4F, 0, 0);
    // keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
    // keybd_event(VK_SHIFT, 0, 0, 0);
  }

  if (psxState.l2 && !prevPsxState_.l2) {
    printf("Press L2");
    keybd_event(VK_DELETE, 0x53, KEYEVENTF_EXTENDEDKEY, 0);
  }

  if (psxState.r2 && !prevPsxState_.r2) {
    printf("Press R2");
    keybd_event(VK_NEXT, 0x51, KEYEVENTF_EXTENDEDKEY, 0);
  }

  if (psxState.select && !prevPsxState_.select) {
    printf("Press Select");
    keybd_event(VK_ESCAPE, 0, 0, 0);
  }

  // Releases
  if (!psxState.north && prevPsxState_.north) {
    printf("Release up");
    releaseKey(VK_UP);
  }

  if (!psxState.west && prevPsxState_.west) {
    printf("Release left");
    releaseKey(VK_LEFT);
  }

  if (!psxState.south && prevPsxState_.south) {
    printf("Release down");
    releaseKey(VK_DOWN);
  }

  if (!psxState.east && prevPsxState_.east) {
    printf("Release right");
    releaseKey(VK_RIGHT);
  }

  if (!psxState.triangle && prevPsxState_.triangle) {
    printf("Release Triangle");
    keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
  }

  if (!psxState.circle && prevPsxState_.circle) {
    printf("Release Circle");
    keybd_event(VK_END, 0x4f, KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY, 0);
  }

  if (!psxState.cross && prevPsxState_.cross) {
    printf("Release Cross");
    keybd_event(VK_RCONTROL, 0, KEYEVENTF_KEYUP, 0);
  }

  if (!psxState.square && prevPsxState_.square) {
    printf("Release Square");
    keybd_event(VK_ADD, 0, KEYEVENTF_KEYUP, 0);
  }

  if (!psxState.l1 && prevPsxState_.l1) {
    printf("Release L1");
    keybd_event(VK_NUMPAD0, 0x52, KEYEVENTF_KEYUP, 0);
  }

  if (!psxState.r1 && prevPsxState_.r1) {
    printf("Release R1");
    keybd_event(VK_NUMPAD1, 0x4F, KEYEVENTF_KEYUP, 0);
    // keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    // keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
  }

  if (!psxState.l2 && prevPsxState_.l2) {
    printf("Release L2");
    keybd_event(VK_DELETE, 0x53, KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY, 0);
  }

  if (!psxState.r2 && prevPsxState_.r2) {
    printf("Release R2");
    keybd_event(VK_NEXT, 0x51, KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY, 0);
  }

  if (!psxState.select && prevPsxState_.select) {
    printf("Release Select");
    keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0);
  }

  printf("\n");

  prevPsxState_ = psxState;
}

void Input::process() {
  HANDLE hGamepadEvent = CreateEvent(NULL, NULL, FALSE, NULL);

  if (!hGamepadEvent)
    throw "Failed to create event";

  pGamepadDeviceOld_->SetEventNotification(hGamepadEvent);

  HRESULT result;
  result = pGamepadDeviceOld_->Acquire();

  if (FAILED(result)) {
    printf("Failed to acquire gamepad!\n");
    throw result;
  }

  printf("Gamepad acquired.\n");

  while (running_) {
    if (WaitForSingleObject(hGamepadEvent, 250) == STATUS_WAIT_0)
      processKeys();

    if (_kbhit())
      if (_getch() == 'q')
        break;
  }

  pGamepadDeviceOld_->Unacquire();

  if (FAILED(result)) {
    printf("Failed unacquire gamepad!\n");
    throw result;
  }

  printf("Gamepad unacquired.\n");

  if (hGamepadEvent)
    CloseHandle(hGamepadEvent);
}

LRESULT CALLBACK Input::_wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_ACTIVATE: {
      break;
    }

    case WM_COMMAND: {
      int wmId = LOWORD(wParam);

      break;
    }

    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      // TODO: Add any drawing code that uses hdc here...
      EndPaint(hWnd, &ps);
      break;
    }

    case WM_DESTROY: {
      PostQuitMessage(0);
      break;
    }

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

BOOL Input::_enumDeviceCallbackOld(LPCDIDEVICEINSTANCE pLpddi, LPVOID pVref) {
  Input* const pThis = static_cast<Input*>(pVref);
  printf("%d: %s", pThis->enumCountOld_, pLpddi->tszInstanceName);

  // TODO: make configurable over ini file:
  if (pThis->enumCountOld_ == 1) {
    HRESULT result{0};

    pThis->pGamepadInstanceOld_ = pLpddi;
    printf(" [selected]\n");

    result = pThis->pInputOld_->CreateDevice(pLpddi->guidInstance, &pThis->pGamepadDeviceOld_, NULL);
    if (FAILED(result)) {
      printf("Failed to create gamepad!\n");
      throw result;
    }

    printf("Gamepad created.\n");

    result = pThis->pGamepadDeviceOld_->SetCooperativeLevel(pThis->hWnd_, DISCL_BACKGROUND | DISCL_EXCLUSIVE);

    if (FAILED(result)) {
      printf("Failed to set cooperation level!\n");
      throw result;
    }

    printf("Cooperation level set.\n");

    result = pThis->pGamepadDeviceOld_->SetDataFormat(&c_dfDIJoystick);

    if (FAILED(result)) {
      printf("Failed to set data format!\n");
      throw result;
    }

    printf("Data format set.\n");

    DIDEVCAPS caps;
    caps.dwSize = sizeof(DIDEVCAPS);
    result = pThis->pGamepadDeviceOld_->GetCapabilities(&caps);

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

  pThis->enumCountOld_++;

  return DIENUM_CONTINUE;
}