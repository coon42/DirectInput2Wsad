#include <stdio.h>
#include <conio.h>
#include <memory>

#include "dualshock2.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------------------
// Application
//-------------------------------------------------------------------------------------------------------------

Application::Application(const Config& cfg) : hInstance_(GetModuleHandle(NULL)), cfg_(cfg) {
  createDummyWindow();
}

Application::~Application() {
  DestroyWindow(hWnd_);
  hWnd_ = 0;
}

void Application::run() {
  std::auto_ptr<GamePad> pGamepad(new DualShock2(hWnd_, cfg_));
  pGamepad->open(1); // TODO: make configurable over ini file

  while (running_) {
    if (pGamepad->waitForButtonEvent(250))
      pGamepad->processButtons();

    if (_kbhit())
      if (_getch() == 'q') {
        printf("\n");
        break;
      }
  }

  pGamepad->close();
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

  Application app(config);
  app.run();
}
