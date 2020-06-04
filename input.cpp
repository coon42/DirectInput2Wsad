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

//-------------------------------------------------------------------------------------------------------------
// DualShock2
//-------------------------------------------------------------------------------------------------------------

DualShock2::DualShock2(HWND hWnd) : GamePad(hWnd), triangle(VK_SPACE),
    circle(VK_END, 0x4f, KEYEVENTF_EXTENDEDKEY), cross(VK_RCONTROL), square(VK_MENU),
    start(188), // ,
    select(VK_ESCAPE),
    l1(VK_NUMPAD0, 0x52),
    l2(190), // , | VK_DELETE, 0x53, KEYEVENTF_EXTENDEDKEY
    r1(VK_SHIFT), // MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC)), // VK_NUMPAD1, 0x4F
    r2(189), // - | VK_NEXT, 0x51, KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY
    north(VK_UP),
    east(VK_RIGHT),
    south(VK_DOWN),
    west(VK_LEFT) {

}

DualShock2::State DualShock2::getButtonState() {
  DIJOYSTATE joyState;
  HRESULT result = pGamepadDevice_->GetDeviceState(sizeof(DIJOYSTATE), &joyState);

  if (FAILED(result))
    throw result;

  return DualShock2::joyState2Psx(joyState);
}

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
// Application
//-------------------------------------------------------------------------------------------------------------

Application::Application() : hInstance_(GetModuleHandle(NULL)) {
  createDummyWindow();
}

Application::~Application() {
  DestroyWindow(hWnd_);
  hWnd_ = 0;
}

void Application::createDummyWindow() {
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

  ShowWindow(hWnd_, SW_HIDE);
  UpdateWindow(hWnd_);
}

void DualShock2::processButtons() {
  printf("event!\n");

  const State state = getButtonState();

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

  printf("\n");

  prevState_ = state;
}

void Application::run() {
  DualShock2 gamepad(hWnd_);
  gamepad.open(1); // TODO: make configurable over ini file

  while (running_) {
    if (gamepad.waitForButtonEvent(250))
      gamepad.processButtons();

    if (_kbhit())
      if (_getch() == 'q')
        break;
  }

  gamepad.close();
}

LRESULT CALLBACK Application::_wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
