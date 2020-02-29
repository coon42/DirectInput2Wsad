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
	LPCDIDEVICEINSTANCE pGamepad_{nullptr};
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
	printf("%i: %s", pThis->enumCount_, pLpddi->tszInstanceName);	

	// TODO: make configrable over ini file:
	if (pThis->enumCount_ == 1) {
		pThis->pGamepad_ = pLpddi;
		printf(" [selected]");
	}
	
	printf("\n");
	pThis->enumCount_++;

	return DIENUM_CONTINUE;
}

int main(const char* pArgs, int argc) {	
	Input input;	
	IDirectInput8* pInput = input.pInput();
	
	input.enumGamepads();
}
