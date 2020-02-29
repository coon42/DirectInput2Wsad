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

private:
	static BOOL _enumDeviceCallback(LPCDIDEVICEINSTANCE pLpddi, LPVOID pVref);
	IDirectInput8* pInput_{nullptr};
	LPCDIDEVICEINSTANCE pGamepadInstance_{nullptr};
	LPDIRECTINPUTDEVICE8 pGamepadDevice_{nullptr};
	int enumCount_{0};
};

Input::Input() {
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
		pThis->pGamepadInstance_ = pLpddi;
		printf(" [selected]\n");

		HRESULT result = pThis->pInput_->CreateDevice(pLpddi->guidInstance, &pThis->pGamepadDevice_, NULL);
		if (FAILED(result)) {
			printf("Failed to create gamepad!\n");
			throw result;
		}

		printf("Gamepad created.\n");

		auto getConsoleHwnd = []() -> HWND {
			char pTitle[256];
			DWORD titleLen = GetConsoleTitle(pTitle, sizeof(pTitle));

			if (!titleLen)
				throw "Console title cannot be obtained!";

			// TODO: set window title to unique name
			HWND hWnd = 0;
			hWnd = FindWindow(NULL, pTitle);

			if (hWnd == INVALID_HANDLE_VALUE)
				throw "Cannot obtain hwnd because window was not found!";

			return hWnd;
		};
		
		result = pThis->pGamepadDevice_->SetCooperativeLevel(getConsoleHwnd(), DISCL_BACKGROUND | DISCL_EXCLUSIVE);

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

int main(const char* pArgs, int argc) {	
	Input input;	
	IDirectInput8* pInput = input.pInput();
	
	input.enumGamepads();
}
