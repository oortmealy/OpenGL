#pragma once
#define PI 3.1415265359
#define RADIAN PI/180.0
#define FACE 0
#define NECK 1
#define SHIRTS 2
#define HAND 3
#define FOOT 3
#define PANTS 4


class Vector3  //3차원 벡터입니다. 다이렉트X처럼 이런거좀 만들어놓았으면 좋겟다.
{
public:
	GLfloat x,y,z;

	Vector3()
		:x(0.f)
		,y(0.f)
		,z(0.f)
	{

	}

	Vector3( GLfloat x, GLfloat y, GLfloat z )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	GLvoid Add(Vector3 * V)
	{
		this->x += V->x;
		this->y += V->y;
		this->z += V->z;
	}
};

GLvoid DrawNet(GLfloat size, GLint LinesX, GLint LinesZ);
GLvoid DrawGuide();
GLvoid DrawMan();
GLuint* GetTexturePool();
GLvoid mySphere(double rx, double ry, double rz) ;
GLvoid myCapsule(double rx, double ry, double rz, double ty) ;
GLvoid myDrawCube (double dx, double dy, double dz) ;
GLbyte *gltLoadTGA(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat);
