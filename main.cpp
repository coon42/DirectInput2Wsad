#include <stdio.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

class Input {
public:
	Input();
	~Input();

	IDirectInput8* pInput() const { return pInput_; }
	void enumGamepads(); // TODO: make private
	void process();

private:
	void createDummyWindow();

	static BOOL _enumDeviceCallback(LPCDIDEVICEINSTANCE pLpddi, LPVOID pVref);
	static LRESULT CALLBACK _wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	IDirectInput8* pInput_{nullptr};
	LPCDIDEVICEINSTANCE pGamepadInstance_{nullptr};
	LPDIRECTINPUTDEVICE8 pGamepadDevice_{nullptr};
	int enumCount_{0};
	HWND hWnd_{0};
	const HINSTANCE hInstance_{0};
	bool running_{true};
};

Input::Input() : hInstance_(GetModuleHandle(NULL)) {
	createDummyWindow();

	HRESULT result = 0;
	result = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8,
			(void**)&pInput_, NULL);

	if (FAILED(result))
		throw result;
}

Input::~Input() {	
	if (pGamepadDevice_) {
		pGamepadDevice_->Release();
		pGamepadDevice_ = nullptr;
	}
	
	if (pInput_) {
		pInput_->Release();
		pInput_ = nullptr;
	}

	DestroyWindow(hWnd_);
	hWnd_ = 0;
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

	ATOM a = RegisterClassEx(&wcex);	
	a = a;

	hWnd_ = CreateWindow(wcex.lpszClassName, "dummy", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
			0, 100, 100, nullptr, nullptr, hInstance_, nullptr);

	if (!hWnd_)
		throw false;

	ShowWindow(hWnd_, SW_SHOW);
	UpdateWindow(hWnd_);
}

void Input::enumGamepads() {
	enumCount_ = 0;

	HRESULT result = 0;
	result = pInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, _enumDeviceCallback, this, DIEDFL_ATTACHEDONLY);
}

BOOL Input::_enumDeviceCallback(LPCDIDEVICEINSTANCE pLpddi, LPVOID pVref) {
	Input* const pThis = static_cast<Input*>(pVref);		
	printf("%d: %s", pThis->enumCount_, pLpddi->tszInstanceName);		

	// TODO: make configurable over ini file:
	if (pThis->enumCount_ == 1) {
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

void Input::process() {
	HANDLE hGamepadEvent = CreateEvent(NULL, NULL, FALSE, NULL);

	if (!hGamepadEvent)
		throw "Failed to create event";

	pGamepadDevice_->SetEventNotification(hGamepadEvent);

	HRESULT result;
	result = pGamepadDevice_->Acquire();

	if (FAILED(result)) {
		printf("Failed to acquire gamepad!\n");
		throw result;
	}

	printf("Gamepad acquired.\n");
	
	while (running_) {		
		if (WaitForSingleObject(hGamepadEvent, INFINITE) == STATUS_WAIT_0)
			printf("event!\n");
	}

	pGamepadDevice_->Unacquire();

	if (FAILED(result)) {
		printf("Failed unacquire gamepad!\n");
		throw result;
	}

	printf("Gamepad unacquired.\n");

	if (hGamepadEvent)
		CloseHandle(hGamepadEvent);
}

int main(const char* pArgs, int argc) {	
	Input input;	
	IDirectInput8* pInput = input.pInput();
	
	input.enumGamepads();
	input.process();
}
