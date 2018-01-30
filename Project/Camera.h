#ifndef CAMERA_H
#define CAMERA_H

//Used for vector & matrix calculations
#include<DirectXMath.h>

//Used for keyboard/mouse input
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>

class Camera {

private:

	//Camera
	DirectX::XMVECTOR camForward;
	DirectX::XMVECTOR camRight;
	DirectX::XMVECTOR camTarget;
	DirectX::XMVECTOR camUp;
	DirectX::XMVECTOR position;

	DirectX::XMMATRIX camRotation;
	DirectX::XMMATRIX camView;

	//Camera movement and rotation values
	float speed;
	float moveX;
	float moveZ;
	float yaw;
	float pitch;

	//Keyboard/Mouse controlls
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	//Previous mouse state and directInput object(used to get input from keyboard)
	DIMOUSESTATE previousMouseState;
	LPDIRECTINPUT8 dI;

public:

	Camera();
	~Camera();

	void getInput(double time);
	void update();
	bool initDI(HINSTANCE hInst, HWND wHandle);

	//Get
	DirectX::XMMATRIX getView(void) const;

};

#endif // !CAMERA_H
