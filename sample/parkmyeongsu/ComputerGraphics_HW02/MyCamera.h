#pragma once
class MyCamera
{
private:
	Vector3 Position;
	Vector3 Forward;
	GLfloat RotatedX, RotatedY;	
	void UpdateForward ( void );
public:
	MyCamera();
	void SetupMatrix ( void );
	void Move ( Vector3 Direction );
	void RotateX ( GLfloat Angle );
	void RotateY ( GLfloat Angle );
	void MoveForward ( GLfloat Distance );
};

