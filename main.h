#ifndef MAIN_H
#define MAIN_H

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
  static LRESULT CALLBACK _wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  HWND hWnd_{0};
  const HINSTANCE hInstance_{0};
  bool running_{true};
};

#endif // MAIN_H
