#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

class Input {
public:
	Input();
	~Input();

private:	
	IDirectInput8* pInput_{nullptr};
};

Input::Input() {
	HRESULT hResult = 0;
	hResult = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pInput_, NULL);

	if (FAILED(hResult))
		throw hResult;
}

Input::~Input() {
	if (pInput_) {
		pInput_->Release();
		pInput_ = nullptr;
	}
}

int main(const char* pArgs, int argc) {	
	Input input;
	
}