#include "StdAfx.h"
#include "MyCamera.h"

MyCamera::MyCamera()
{
	RotatedX = RotatedY = 0.0;
	UpdateForward();
}

void MyCamera::UpdateForward( void )
{
	Vector3 vecY, vecXY;
	vecY.x = cos( (RotatedY + 90.0) * RADIAN);
	vecY.z = -sin( (RotatedY + 90.0) * RADIAN);
	double cosX = cos (RotatedX * RADIAN);
	vecXY.x = vecY.x * cosX;
	vecXY.z = vecY.z * cosX;
	vecXY.y = sin(RotatedX * RADIAN);
	Forward = vecXY;
}

void MyCamera::Move (Vector3 Direction)
{
	Position.Add( &Direction );
}

void MyCamera::RotateY (GLfloat Angle)
{
	RotatedY += Angle;
	UpdateForward();
}

void MyCamera::RotateX (GLfloat Angle)
{
	RotatedX += Angle;
	UpdateForward();
}

void MyCamera::SetupMatrix( void )
{
	//gluLookAt(Position.x,Position.y,Position.z,
	glTranslatef( -Position.x, -Position.y, -Position.z );
	glRotatef(-RotatedX , 1.0, 0.0, 0.0);
	glRotatef(-RotatedY , 0.0, 1.0, 0.0);
}

void MyCamera::MoveForward( GLfloat Distance )
{
	Vector3 Dir;
	Dir = Position;
	GLfloat Norm = sqrtf(Dir.x*Dir.x +Dir.y*Dir.y +Dir.z*Dir.z);
	if(Norm < 1.0f && Distance <0.0f)
		return;

	Dir.x /= Norm;
	Dir.y /= Norm;
	Dir.z /= Norm;
	Vector3 MoveVector;
	MoveVector.x = Dir.x * Distance;//Forward.x * -Distance;
	MoveVector.y = Dir.y * Distance; //Forward.y * -Distance;
	MoveVector.z = Dir.z * Distance; //Forward.z * -Distance;
	Position.Add( &MoveVector );
}