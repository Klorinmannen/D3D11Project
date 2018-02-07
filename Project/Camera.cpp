#include"Camera.h"

Camera::Camera() {

	//Camera
	this->camForward = { 0.0f, 0.0f, 1.0f, 0.0f };
	this->camRight = { 1.0f, 0.0f, 0.0f, 0.0f };
	this->camUp = { 0.0f, 1.0f, 0.0f, 0.0f };

	this->camTarget = { 0.0f, 0.0f, 0.0f, 0.0f };
	this->position = { 0.0f, 0.0f, -2.0f, 0.0f };

	this->camRotation = DirectX::XMMatrixIdentity();
	this->camView = DirectX::XMMatrixIdentity();

	this->speed = 0.0f;
	this->moveX = 0.0f;
	this->moveZ = 0.0f;
	this->yaw = 0.0f;
	this->pitch = 0.0f;

	//Keyboard and mouse
	this->keyboard = nullptr;
	this->mouse = nullptr;

}

Camera::~Camera() {

	//Unaquire mouse/keyboard from this application
	this->keyboard->Unacquire();
	this->mouse->Unacquire();


	this->keyboard->Release();
	this->mouse->Release();
	this->dI->Release();

}

void Camera::getInput() {

	//Stores mouse/keyboard info
	DIMOUSESTATE currentMouseState;
	BYTE keyboardState[256];

	//Acuires the mouse and keyboard for this application(incase it is used elsewhere)
	this->keyboard->Acquire();
	this->mouse->Acquire();

	//Get current state of mouse/keyboard
	this->keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
	this->mouse->GetDeviceState(sizeof(DIMOUSESTATE), &currentMouseState);

	//Update speed value
	this->speed = 1.0f * 0.0005f;
	bool x = false, z = false;

	//Keyboard input
	if (keyboardState[DIK_A] & 0x80) {

		this->moveX -= this->speed;
		x = true;

	}
	else if (keyboardState[DIK_D] & 0x80) {

		this->moveX += this->speed;
		x = true;

	}

	if (keyboardState[DIK_W] & 0x80) {

		this->moveZ += this->speed;
		z = true;
		
	}
	else if (keyboardState[DIK_S] & 0x80) {

		this->moveZ -= this->speed;
		z = true;

	}

	//Keeps same speed when moving in one or two directions
	if (x && z) {

		this->moveX /= 2;
		this->moveZ /= 2;

	}

	//Check mouse input
	if (currentMouseState.lX != this->previousMouseState.lX) {

		this->yaw += currentMouseState.lX * 0.005f;

		//Save previous mouse state
		this->previousMouseState = currentMouseState;

	}

	if (currentMouseState.lY != this->previousMouseState.lY) {

		this->pitch += currentMouseState.lY * 0.005f;

		//Save previous mouse state
		this->previousMouseState = currentMouseState;

	}

}

void Camera::update() {

	//Rotation matrix, rotates X, Y & Z
	this->camRotation = DirectX::XMMatrixRotationRollPitchYaw(this->pitch, this->yaw, 0.0f);
	
	//Transforms defaultForward vector with rotation matrix
	this->camTarget = DirectX::XMVector3TransformCoord({ 0.0f, 0.0f, 1.0f, 0.0f }, this->camRotation);

	//Normalize camTarget vector
	this->camTarget = DirectX::XMVector3Normalize(this->camTarget);

	//Transform camera vectors with rotation matrix
	this->camRight = DirectX::XMVector3TransformCoord({ 1.0f, 0.0f, 0.0f, 0.0f }, this->camRotation);
	this->camForward = DirectX::XMVector3TransformCoord({ 0.0f, 0.0f, 1.0f, 0.0f }, this->camRotation);
	this->camUp = DirectX::XMVector3Cross(this->camForward, this->camRight);

	//Apply movement to the camera
	this->position = DirectX::XMVectorAdd(DirectX::XMVectorScale(this->camRight, this->moveX), this->position);
	this->position = DirectX::XMVectorAdd(DirectX::XMVectorScale(this->camForward, this->moveZ), this->position);

	//Reset movement values
	this->moveX = 0.0f;
	this->moveZ = 0.0f;

	//Add the position to the camera target matrix
	this->camTarget = DirectX::XMVectorAdd(this->position, this->camTarget);

	//Set values for the camView matrix which is the final view
	this->camView = DirectX::XMMatrixLookAtLH(this->position, this->camTarget, this->camUp);

}

bool Camera::initDI(HINSTANCE hInst, HWND wHandle) {

	bool result = true;

	//Create direct input object
	HRESULT hr = DirectInput8Create(
		hInst,				//Application instance
		DIRECTINPUT_VERSION,//Version of DirectInput to use
		IID_IDirectInput8,	//Interface of DirectInput to use
		(void**)&this->dI,	//Pointer to DirectInput object
		NULL);				//COM

	if (FAILED(hr)) {

		result = false;

	}
	else {

		//Create Keyboard
		hr = this->dI->CreateDevice(
		GUID_SysKeyboard,	//GUID Flag
		&this->keyboard,	//Device to use
		NULL				//COM
		);

		if (FAILED(hr)) {

			result = false;

		}
		else {

			//Create Mouse
			hr = this->dI->CreateDevice(
				GUID_SysMouse,	//GUID Flag
				&this->mouse,	//Device to use
				NULL			//COM
			);

			if (FAILED(hr)) {

				result = false;

			}
			else {

				//Set data format
				this->keyboard->SetDataFormat(&c_dfDIKeyboard);	//Set type of device
				this->keyboard->SetCooperativeLevel(wHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);	//Flags

				this->mouse->SetDataFormat(&c_dfDIMouse);		//Set type of device
				this->mouse->SetCooperativeLevel(wHandle, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);	//Flags

			}

		}

	}

	return result;

}

DirectX::XMMATRIX Camera::getView(void) const{

	return this->camView;

}