// ComputerGraphics_HW02.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "MyCamera.h"

void reshape(int x, int y);
void Display(void);
void MouseDrag(int x, int y);
void MouseMove(int button, int state, int x, int y);
void KeyDown(unsigned char key, int x, int y);
void Init();

MyCamera Camera;
int _button = -1;
int _state = -1;
int X = 0;
int Y = 0;

int _tmain(int argc, _TCHAR* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800,600);
	glutCreateWindow("ComputerGraphics_HW02");
	Camera.Move( Vector3(0.0, -1.5, 8.0 ));
	glutDisplayFunc(Display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(KeyDown);
	glutMouseFunc(MouseMove);
	glutMotionFunc(MouseDrag);
	Init();
	glutMainLoop();
	return 0;             
}

void Init()
{
    GLbyte *pBytes;                     
    GLint iComponents, iWidth, iHeight; 
    GLenum eFormat;                     
    
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);	
    glEnable(GL_CULL_FACE);	
	glEnable(GL_TEXTURE_2D);
    
    glGenTextures(20, GetTexturePool());
    ///////////////////////////////////////////
    // 머리
    glBindTexture(GL_TEXTURE_2D, GetTexturePool()[FACE]);
    pBytes = gltLoadTGA("face.tga", &iWidth, &iHeight, &iComponents, &eFormat);    
    glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, (GLvoid *)pBytes);
    free(pBytes);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    ///////////////////////////////////////////
    // 목
	
    glBindTexture(GL_TEXTURE_2D, GetTexturePool()[NECK]);
    pBytes = gltLoadTGA("neck.tga", &iWidth, &iHeight, &iComponents, &eFormat);    
    glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, (GLvoid *)pBytes);
    free(pBytes);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    ///////////////////////////////////////////
    //티셔츠
	
    glBindTexture(GL_TEXTURE_2D, GetTexturePool()[SHIRTS]);
    pBytes = gltLoadTGA("shirts.tga", &iWidth, &iHeight, &iComponents, &eFormat);    
    glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, (GLvoid *)pBytes);
    free(pBytes);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	///////////////////////////////////////////
    //손
	
    glBindTexture(GL_TEXTURE_2D, GetTexturePool()[HAND]);
    pBytes = gltLoadTGA("hand.tga", &iWidth, &iHeight, &iComponents, &eFormat);    
    glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, (GLvoid *)pBytes);
    free(pBytes);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	///////////////////////////////////////////
    //바지
	
	glBindTexture(GL_TEXTURE_2D, GetTexturePool()[PANTS]);
    pBytes = gltLoadTGA("pants.tga", &iWidth, &iHeight, &iComponents, &eFormat);    
    glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, (GLvoid *)pBytes);
    free(pBytes);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glDisable(GL_TEXTURE_2D);
}

void reshape(int x, int y)
{
	if (y == 0 || x == 0) return;
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();
	gluPerspective(45.0,(GLdouble)x/(GLdouble)y,0.1,1000.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,x,y);
}

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	Camera.SetupMatrix();
	//DrawGuide();
	glColor3f(1.0,1.0,1.0);
	//DrawNet(1,10,10);
	    glEnable(GL_TEXTURE_2D);
	DrawMan();
		glDisable(GL_TEXTURE_2D);
	glFlush();  
	glutSwapBuffers();
}

void MouseDrag(int x, int y)
{
	if(_button == GLUT_LEFT_BUTTON && _state == GLUT_DOWN)
	{
		int offsetX, offsetY;
		offsetX = x - X;
		offsetY = y - Y;
		Camera.RotateY(offsetX * 0.3f);
		Camera.RotateX(offsetY * 0.3f);
		X = x;
		Y = y;
		Display();
	}
}

void MouseMove(int button, int state, int x, int y)
{
	_button = button;
	_state = state;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		X = x;
		Y = y;
	}
}

void KeyDown(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case 27:		//ESC
		PostQuitMessage(0);
		break;
	case 'a':		
		Camera.MoveForward( -0.1 ) ;
		Display();
		break;
	case 'z':		
		Camera.MoveForward( 0.1 ) ;
		Display();
		break;
	}
}