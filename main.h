#ifndef MAIN_H
#define MAIN_H

//-------------------------------------------------------------------------------------------------------------
// Application
//-------------------------------------------------------------------------------------------------------------

class Application {
public:
  Application(const Config& cfg);
  ~Application();

  void run();

private:
  void createDummyWindow();
  static LRESULT CALLBACK _wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  HWND hWnd_{0};
  const HINSTANCE hInstance_{0};
  bool running_{true};
  const Config cfg_;
};

#endif // MAIN_H
