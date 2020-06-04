#include <stdio.h>
#include <conio.h>

#include "gamepad.h"
#include "main.h"

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

//-------------------------------------------------------------------------------------------------------------
// main
//-------------------------------------------------------------------------------------------------------------

int main(const char* pArgs, int argc) {
  // TODO: move Config to Application class:
  Config config("config.ini");

  Application app;
  app.run();
}