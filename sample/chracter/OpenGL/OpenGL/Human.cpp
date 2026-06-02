#include <Windows.h>
#include <stdlib.h> 
#include <math.h> 
#include <stdio.h>
#include <GL\GLU.h>
#include <GL\glut.h>
#include <GL\glaux.H>
#include <direct.h>

#define COLOR_TEXTURE_NUM 16
#define IMAGE_TEXTURE_NUM 16

#define LONGITUDINAL_LINE 20
#define LATITUDINAL_LINE 20
#define CALRGB(num) num*1.0/255
#define CALVECTOR(num) num*1.0/500

#define ZERO 0
#define INT_ONE 1
#define FlOAT_ONE 1.0
#define ESC 27
#define GL_PI 3.1415f

void resizeWindow(int w, int h);
void glInit();
void InitLight();
void keyEventHandler(int key, int x, int y);

char colorBitmapName[COLOR_TEXTURE_NUM][200] = { "redColor.bmp", "wineColor.bmp", "orangeColor.bmp", "yellowColor.bmp", "yellowColor.bmp", "greenColor.bmp", "skinColor.bmp", "blackColor.bmp", "sunColor.bmp", "hairColor.bmp", "grassColor.bmp","jeanColor.bmp", "checkColor.bmp" , "ddangColor.bmp", "glassColor.bmp", "shoeColor.bmp"};
char imageBitmapName[IMAGE_TEXTURE_NUM][200] = { "redColor.bmp", "wineColor.bmp", "orangeColor.bmp", "yellowColor.bmp", "yellowColor.bmp", "greenColor.bmp", "skinColor.bmp", "blackColor.bmp", "sunColor.bmp", "hairColor.bmp", "grassColor.bmp","jeanColor.bmp", "checkColor.bmp" , "ddangColor.bmp", "glassColor.bmp", "shoeColor.bmp"};
 
GLuint colorTexture[COLOR_TEXTURE_NUM];
GLuint imageTexture[IMAGE_TEXTURE_NUM];

GLUquadricObj *imageObject[IMAGE_TEXTURE_NUM];

float cameraX;
float cameraY;
float cameraZ;

float cameraRotationX;
float cameraRotationY;

float  radius = 3.0f;
float  theta = 0.1f;
float  phi = -1.4f;

float x = 0.0, y = -5.0; 
float deltaMove = 0.0; 

float lx = 0.0, ly = 1.0;
float angle = 0.0; 

void update(void)
{
	if (deltaMove) { 
		x += deltaMove * lx * 0.1;
		y += deltaMove * ly * 0.1;
	}
	glutPostRedisplay(); 
}


void processNormalKeys(unsigned char key, int xx, int yy)
{
	if (key == ESC || key == 'q' || key == 'Q') exit(0);

	else if (key == 'a' && radius < 10) {
		radius = radius + 0.5;
	}
	else if (key == 'z' && radius > 1.5) {
		radius = radius - 0.5;
	}
}

typedef struct ROTATE {
	GLfloat angle;
	GLfloat x;
	GLfloat y;
	GLfloat z;
}ROTATE;

typedef struct TRANS {
	GLfloat x;
	GLfloat y;
	GLfloat z;
}TRANS;

typedef struct SCALA {
	GLfloat x;
	GLfloat y;
	GLfloat z;
}SCALA;

typedef struct RGB{
	GLfloat red;
	GLfloat green;
	GLfloat blue;
}RGB;
/* 비트맵 이미지 읽어오는 함수 */
AUX_RGBImageRec *LoadBMP(const char *Filename)                      
{
	FILE *File = NULL;                                                
	if (!Filename)                                                 
	{
		return NULL;                                                
	}
	File = fopen(Filename, "r");                                       
	if (File)                                                       
	{
		fclose(File);                                               
		return auxDIBImageLoad(Filename);                          
	}
	return NULL;                                                    
}
void LoadGLColorTextures()  
{
		AUX_RGBImageRec *TextureImage;                    
	
		for (int i = 0; i < COLOR_TEXTURE_NUM; i++) {
			if (TextureImage = LoadBMP(colorBitmapName[i]))
			{
				glGenTextures(1, &colorTexture[i]);
													 
				glBindTexture(GL_TEXTURE_2D, colorTexture[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX, TextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);    
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
			}
			if (TextureImage)                            
			{
				if (TextureImage->data)                   
				{
					free(TextureImage->data);              
				}
				free(TextureImage);                       
			}
		}
                              
}

void LoadGLImageTextures() {
	
	AUX_RGBImageRec *tempBMP;

	for (int i = 0; i < IMAGE_TEXTURE_NUM; i++) {
		imageObject[i] = NULL;
		imageObject[i] = gluNewQuadric();
		gluQuadricDrawStyle(imageObject[i], GLU_FILL);
		gluQuadricTexture(imageObject[i], GL_TRUE);
	}

	for (int i = 0; i < IMAGE_TEXTURE_NUM; i++) {
		if (tempBMP = LoadBMP(imageBitmapName[i]))
		{
			glGenTextures(1, &imageTexture[i]);

			glBindTexture(GL_TEXTURE_2D, imageTexture[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, tempBMP->sizeX, tempBMP->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBMP->data);


			if (tempBMP)
			{
				if (tempBMP->data)
				{
					free(tempBMP->data);
				}
				free(tempBMP);
			}
		}
	}

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void drawHalfSphere(TRANS trans, GLfloat radius, int bmpNum)
{

	GLfloat angley;
	GLfloat nexty;
	GLfloat anglex;
	GLfloat x;
	GLfloat y;
	GLfloat z;
	
	glBindTexture(GL_TEXTURE_2D, colorTexture[bmpNum]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glBegin(GL_QUAD_STRIP);

	for (angley = 0.0f; angley <= (0.5f*GL_PI); angley += ((0.5f*GL_PI) / 8.0f))
	{
		y = radius*sin(angley); 
		nexty = angley + ((0.5f*GL_PI) / 8.0f);		
		for (anglex = 0.0f; anglex < (2.0f*GL_PI); anglex += (GL_PI / 8.0f))
		{
			x = radius*cos(angley)*sin(anglex);
			z = radius*cos(angley)*cos(anglex);
			glNormal3f(-cos(angley)*sin(anglex), -sin(angley), -cos(angley)*cos(anglex)); 
			glTexCoord2f(trans.x + x, trans.y + y); glVertex3f(trans.x + x, trans.y + y , z);
			

			x = radius*cos(nexty)*sin(anglex);
			z = radius*cos(nexty)*cos(anglex);
			glNormal3f(-cos(nexty)*sin(anglex), -sin(nexty), -cos(nexty)*cos(anglex));
			glTexCoord2f(trans.x + x, trans.y + y); glVertex3f(trans.x + x, trans.y + radius*sin(nexty), z);

			glColor3f(0.0, 0.0, 1.0); 
		}
	}
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}


void drawCylinder(RGB rgb, TRANS trans, ROTATE rotate, GLfloat radius, GLfloat h, int bmpNum)
{
	GLfloat centerx = trans.x;
	GLfloat centery = trans.y;
	GLfloat centerz = trans.z;

	glBindTexture(GL_TEXTURE_2D, colorTexture[bmpNum]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glRotatef(rotate.angle, rotate.x, rotate.y, rotate.z);
	glTranslated(trans.x, trans.y, trans.z);
	GLfloat x, y, angle;

	glBegin(GL_TRIANGLE_FAN);           //원기둥의 윗면
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 1.1f);
	glVertex3f(centerx, centery, centerz);

		for (angle = 0.0f; angle < (2.0f*GL_PI); angle += (GL_PI / 8.0f))
		{
			x = centerx + radius*sin(angle);
			y = centery + radius*cos(angle);
			glNormal3f(0.0f, 0.0f, -1.0f);
			glTexCoord2f(0.0f, 1.1f);
			glVertex3f(x, y, centerz);
		}
	glEnd();

	glBegin(GL_QUAD_STRIP);            //원기둥의 옆면
	for (angle = 0.0f; angle < (2.0f*GL_PI); angle += (GL_PI / 8.0f))
	{
		x = centerx + radius*sin(angle);
		y = centery + radius*cos(angle);
		glNormal3f(sin(angle), cos(angle), 0.0f);
		glTexCoord2f(1.0/17 + x, 0.0f);
		glVertex3f(x, y, centerz);
		glTexCoord2f(1.0/17 + x, 1.0f);
		glVertex3f(x, y, centerz + h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);           //원기둥의 밑면
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(centerx, centery, centerz + h);
	for (angle = (2.0f*GL_PI); angle > 0.0f; angle -= (GL_PI / 8.0f))
	{
		x = centerx + radius*sin(angle);
		y = centery + radius*cos(angle);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 1.f);
		glVertex3f(x, y, centerz + h);
	}
	glEnd();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
void drawRectangle(TRANS trans, int bmpNum) {
	glBindTexture(GL_TEXTURE_2D, imageTexture[bmpNum]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	
	glTranslated(trans.x, trans.y, trans.z);
	glBegin(GL_POLYGON);

	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);

	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	return;
}

void drawShoe(TRANS trans, int bmpNum) {
	glBindTexture(GL_TEXTURE_2D, imageTexture[bmpNum]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();

	glTranslated(trans.x, trans.y - 0.022, trans.z);
	glScaled(0.08, 0.05, 0.04);
	glRotated(90, 0, 1, 0);
	glBegin(GL_POLYGON);

	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);

	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	return;
}

void drawCube(RGB rgb, TRANS trans, SCALA scale, ROTATE rotate, int bmpNum) {
	
	glBindTexture(GL_TEXTURE_2D, imageTexture[bmpNum]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();

	glTranslated(trans.x, trans.y, trans.z);
	glScalef(scale.x/10, scale.y/10, scale.z/10);
	glRotatef(rotate.angle, rotate.x, rotate.y, rotate.z);
	
	glBegin(GL_QUADS);
	
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // { Front }
	glTexCoord3f(0.0f, 1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // { Front }
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f); // { Front }
	glTexCoord3f(1.0f, 0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f); // { Front }

	glTexCoord3f(1.0f, 0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f); // { Right }
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f); // { Right }
	glTexCoord3f(0.0f, 1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f); // { Right }
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, 1.0f, -1.0f); // { Right }

	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, 1.0f, -1.0f); // { Back }
	glTexCoord3f(0.0f, 1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f); // { Back }
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // { Back }
	glTexCoord3f(1.0f, 0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // { Back }

	glTexCoord3f(1.0f, 0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // { Left }
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // { Left }
	glTexCoord3f(0.0f, 1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // { Left }
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // { Left }

	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f); // { Top }
	glTexCoord3f(0.0f, 1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f); // { Top }
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f); // { Top }
	glTexCoord3f(1.0f, 0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f); // { Top }

	glTexCoord3f(1.0f, 0.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f); // { Bottom }
	glTexCoord3f(0.0f, 0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f); // { Bottom }
	glTexCoord3f(0.0f, 1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // { Bottom }
	glTexCoord3f(1.0f, 0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f); // { Bottom }

	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	return;
}

void drawSphere(RGB rgb, TRANS trans, GLfloat radis, int bmpNum) {
	
	
	glBindTexture(GL_TEXTURE_2D, colorTexture[bmpNum]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);
	
	glPushMatrix();
	glTranslated(trans.x, trans.y, trans.z);

	int i, j;
	const float PI = 3.14159265358979f;
	const float TWOPI = 6.28318530717958f;
	const float PIDIV2 = 1.57079632679489f;

	float theta1 = 0.0;
	float theta2 = 0.0;
	float theta3 = 0.0;

	float ex = 0.0f;
	float ey = 0.0f;
	float ez = 0.0f;

	float px = 0.0f;
	float py = 0.0f;
	float pz = 0.0f;

	int p = 30;

	if (p < 0)
		p = -p;
	GLfloat cx = trans.x;
	GLfloat cy = trans.y;
	GLfloat cz = trans.z;

	if (p < 4 || radis <= 0) {
		glBegin(GL_POINTS);
		glVertex3f(cx, cy, cz);
		glEnd();
		return;
	}

	for (i = 0; i < p / 2; ++i) {
		theta1 = i * TWOPI / p - PIDIV2;
		theta2 = (i + 1) * TWOPI / p - PIDIV2;

		glBegin(GL_TRIANGLE_STRIP);
		{
			for (j = 0; j <= p; ++j)
			{
				theta3 = j * TWOPI / p;

				ex = cos(theta2) * cos(theta3);
				ey = sin(theta2);
				ez = cos(theta2) * sin(theta3);
				px = cx + radis * ex;
				py = cy + radis * ey;
				pz = cz + radis * ez;

				glNormal3f(ex, ey, ez);
				glTexCoord2f(-(j / (float)p), 2 * (i + 1) / (float)p);
				glVertex3f(px, py, pz);

				ex = cos(theta1) * cos(theta3);
				ey = sin(theta1);
				ez = cos(theta1) * sin(theta3);
				px = cx + radis * ex;
				py = cy + radis * ey;
				pz = cz + radis * ez;

				glNormal3f(ex, ey, ez);
				glTexCoord2f(-(j / (float)p), 2 * i / (float)p);
				glVertex3f(px, py, pz);
			}
		}
		glEnd();
	}

	glPopMatrix();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	return;
}

void drawCone(RGB rgb, TRANS trans, ROTATE rotate, GLfloat radis, GLfloat height, int bmpNum) {
	glBindTexture(GL_TEXTURE_2D, colorTexture[bmpNum]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	glColor3f(rgb.red, rgb.green, rgb.blue);
	glTranslated(trans.x, trans.y, trans.z);
	glRotatef(rotate.angle, rotate.x, rotate.y, rotate.z);

	glBegin(GL_TRIANGLES);

	glTexCoord3f(0.0f, 0.5f, 0.0f); glVertex3f(0.0f, 0.05f, 0.0f); 
	glTexCoord3f(-0.5f, -0.5f, 0.5f);	glVertex3f(-0.05f, -0.05f, 0.05f); 
	glTexCoord3f(0.5f, -0.5f, 0.5f);	glVertex3f(0.05f, -0.05f, 0.05f); 

	glTexCoord3f(0.0f, 0.5f, 0.0f);	glVertex3f(0.0f, 0.05f, 0.0f); 
	glTexCoord3f(0.5f, -0.5f, 0.5f);	glVertex3f(0.05f, -0.05f, 0.05f); 
	glTexCoord3f(0.5f, -0.5f, -0.5f);	glVertex3f(0.05f, -0.05f, -0.05f); 

	glTexCoord3f(0.0f, 0.05f, 0.0f);	glVertex3f(0.0f, 0.05f, 0.0f); 
	glTexCoord3f(0.5f, -0.5f, -0.5f);	glVertex3f(0.05f, -0.05f, -0.05f); 
	glTexCoord3f(-0.5f, -0.5f, -0.5f);	glVertex3f(-0.05f, -0.05f, -0.05f); 

	glTexCoord3f(0.0f, 0.5f, 0.0f);	glVertex3f(0.0f, 0.05f, 0.0f);
	glTexCoord3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.05f, -0.05f, -0.05f);
	glTexCoord3f(-0.5f, -0.5f, 0.5f); glVertex3f(-0.05f, -0.05f, 0.05f); 
	
	glEnd();
	
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	return;
}

static void drawTorus(RGB rgb, TRANS trans, GLfloat numc, GLfloat numt, ROTATE rotate, int bmpNum, int sizeX, int sizeY, int sizeZ)
{
	glBindTexture(GL_TEXTURE_2D, colorTexture[bmpNum]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	glColor3f(rgb.red, rgb.green, rgb.blue);
	glTranslated(trans.x, trans.y, trans.z);
	glRotatef(rotate.angle, rotate.x, rotate.y, rotate.z);

	int i, j, k;
	double s, t, x, y, z, twopi;

	twopi = 2 * (double)GL_PI;
	for (i = 0; i < numc; i++) {
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= numt; j++) {
			for (k = 1; k >= 0; k--) {
				s = (i + k) % (int)numc;
				t = j % (int)numt;

				x = (1 + .1*cos(s*twopi / numc))*cos(t*twopi / numt);
				y = (1 + .1*cos(s*twopi / numc))*sin(t*twopi / numt);
				z = .1 * sin(s * twopi / numc);
				glTexCoord2f(1.0 / numt + x, 1.0f);
				glVertex3f(x/sizeX, y/sizeY, z/sizeZ);
			}
		}
		glEnd();
	}

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

static void drawHalfTorus(RGB rgb, TRANS trans, GLfloat numc, GLfloat numt, ROTATE rotate, int bmpNum, int sizeX, int sizeY, int sizeZ)
{
	glBindTexture(GL_TEXTURE_2D, colorTexture[bmpNum]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	glColor3f(rgb.red, rgb.green, rgb.blue);
	glTranslated(trans.x, trans.y, trans.z);
	glRotatef(rotate.angle, rotate.x, rotate.y, rotate.z);

	int i, j, k;
	double s, t, x, y, z, twopi;

	twopi = 2 * (double)GL_PI;
	for (i = 0; i < numc; i++) {
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= numt /2; j++) {
			for (k = 1; k >= 0; k--) {
				s = (i + k) % (int)numc;
				t = j % (int)numt;

				x = (1 + .1*cos(s*twopi / numc))*cos(t*twopi / numt);
				y = (1 + .1*cos(s*twopi / numc))*sin(t*twopi / numt);
				z = .1 * sin(s * twopi / numc);
				glVertex3f(x / sizeX, y / sizeY, z / sizeZ);
			}
		}
		glEnd();
	}

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void drawHair() {

	RGB  hairColor = { CALRGB(255), CALRGB(255), CALRGB(255) };
	
	TRANS hairTrans = { CALVECTOR(-165) , CALVECTOR(65) , CALVECTOR(0) };
	TRANS hairBandLeft1Trans = { hairTrans.x + CALVECTOR(50) , hairTrans.y + CALVECTOR(-10) , hairTrans.z + CALVECTOR(0) };
	TRANS hairBandLeft2Trans = { hairTrans.x + CALVECTOR(50) , hairTrans.y + CALVECTOR(-23) , hairTrans.z + CALVECTOR(0) };
	TRANS hairBandRight1Trans = { hairTrans.x + CALVECTOR(120) , hairTrans.y + CALVECTOR(-10) , hairTrans.z + CALVECTOR(0) };
	TRANS hairBandRight2Trans = { hairTrans.x + CALVECTOR(120) , hairTrans.y + CALVECTOR(-23) , hairTrans.z + CALVECTOR(0) };
	
	TRANS hairLeftTrans = { hairTrans.x + CALVECTOR(-90) , hairTrans.y + CALVECTOR(30) , hairTrans.z + CALVECTOR(0) };
	TRANS hairRightTrans = { hairTrans.x + CALVECTOR(90) , hairTrans.y + CALVECTOR(30) , hairTrans.z + CALVECTOR(0) };
	ROTATE hairLeftRotate = { 180, 1, 1, 0 };
	ROTATE hairRightRotate = { 180, 1, -1, 0 };
	
	drawHalfSphere(hairTrans, CALVECTOR(70), 9);
	drawSphere(hairColor, hairBandLeft1Trans, CALVECTOR(15), 0);
	drawSphere(hairColor, hairBandLeft2Trans, CALVECTOR(15), 0);
	drawSphere(hairColor, hairBandRight1Trans, CALVECTOR(15), 0);
	drawSphere(hairColor, hairBandRight2Trans, CALVECTOR(15), 0);
	drawCone(hairColor, hairLeftTrans, hairLeftRotate, CALVECTOR(25), CALVECTOR(40), 9);
	drawCone(hairColor, hairRightTrans, hairRightRotate, CALVECTOR(25), CALVECTOR(40), 9);

};
void drawFace() {

	RGB faceColor = { CALRGB(255), CALRGB(220), CALRGB(177) };
	RGB glassColor = { CALRGB(255), CALRGB(255), CALRGB(255) };
	RGB cheekColor = { CALRGB(255), CALRGB(255), CALRGB(255) };
	RGB lipColor = { CALRGB(255), CALRGB(255), CALRGB(255) };

	TRANS faceTrans = { CALVECTOR(-165) , CALVECTOR(60) , CALVECTOR(0) };
	TRANS faceCenterTrans = { CALVECTOR(82) + faceTrans.x, CALVECTOR(-30) + faceTrans.y,faceTrans.z };

	TRANS cheekLeftTrans = { CALVECTOR(-35) + faceTrans.x, CALVECTOR(-30) + faceTrans.y, CALVECTOR(-60) + faceTrans.z };
	TRANS cheekRightTrans = { CALVECTOR(35) + faceTrans.x, CALVECTOR(-30) + faceTrans.y, CALVECTOR(-60) + faceTrans.z };

	TRANS lipTrans = { CALVECTOR(0) + faceTrans.x, CALVECTOR(-40) + faceTrans.y, CALVECTOR(-60) + faceTrans.z };

	TRANS glassLeftTrans = { CALVECTOR(-30) + faceTrans.x, CALVECTOR(0) + faceTrans.y, CALVECTOR(-68) + faceTrans.z };
	TRANS glassRightTrans = { CALVECTOR(30) + faceTrans.x, CALVECTOR(0) + faceTrans.y, CALVECTOR(-68) + faceTrans.z };
	TRANS glassCenterTrans = { CALVECTOR(197) + faceTrans.x, CALVECTOR(-30) + faceTrans.y, CALVECTOR(-88) + faceTrans.z };


	TRANS eyeLeftTrans = { CALVECTOR(-30) + faceTrans.x, CALVECTOR(-7) + faceTrans.y, CALVECTOR(-68) + faceTrans.z };
	TRANS eyeRightTrans = { CALVECTOR(30) + faceTrans.x, CALVECTOR(-7) + faceTrans.y, CALVECTOR(-68) + faceTrans.z };

	SCALA cheekLeftrScala = { CALVECTOR(100), CALVECTOR(50), CALVECTOR(50) };
	SCALA cheekRightScala = { CALVECTOR(100), CALVECTOR(50), CALVECTOR(50) };
	SCALA lipScala = { CALVECTOR(100), CALVECTOR(150), CALVECTOR(50) };

	ROTATE cheekLeftRotate = { 0, 0, 0, 0 };
	ROTATE cheekRightRotate = { 0, 0, 0, 0 };

	ROTATE glassLeftRotate = { 0, 0, 0, 0 };
	ROTATE glassRightRotate = { 0, 0, 0, 0 };
	
	ROTATE lipRotate = { 0, 0, 0, 0 };
	
	ROTATE noseRotate = { 90, -1, 0, 0 };
	ROTATE grassRotate = { 90, 0, 1, 0 };

	drawSphere(faceColor, faceCenterTrans, CALVECTOR(65),6);
	
	drawCube(cheekColor, cheekLeftTrans, cheekLeftrScala, cheekLeftRotate, 1);
	drawCube(cheekColor, cheekRightTrans, cheekRightScala, cheekRightRotate, 1);
	
	drawCube(lipColor, lipTrans, lipScala, lipRotate, 0);

	drawCylinder(glassColor, glassCenterTrans, grassRotate, CALVECTOR(5), CALVECTOR(22), 10);

	drawTorus(glassColor, glassRightTrans, 4, 20, glassRightRotate, 10, 23,23,23);
	drawTorus(glassColor, glassLeftTrans, 4, 20, glassLeftRotate, 10, 23,23,23);

	drawHalfTorus(glassColor, eyeRightTrans, 4, 20, glassLeftRotate, 7, 40, 40, 40);
	drawHalfTorus(glassColor, eyeLeftTrans, 4, 20, glassLeftRotate, 7, 40, 40, 40);
};

void drawNeck() {
	RGB neckColor = { CALRGB(255), CALRGB(220), CALRGB(177) };
	TRANS neckTrans = { CALVECTOR(-83) , CALVECTOR(0) , CALVECTOR(0) };
	ROTATE neckRotate = { 90, 1, 0, 0 };

	drawCylinder(neckColor, neckTrans, neckRotate, CALVECTOR(20), CALVECTOR(30),6);
};

void drawHead() {
	drawHair();
	drawFace();
	drawNeck();
};

void drawLeftArm() {
	RGB leftArmColor = { CALRGB(255), CALRGB(220), CALRGB(177) };
	TRANS leftArmTrans = { CALVECTOR(-80) , CALVECTOR(-350) , CALVECTOR(0) };

	TRANS leftArmTrans_hand = { leftArmTrans.x + CALVECTOR(40) , leftArmTrans.y + CALVECTOR(270) , leftArmTrans.z + CALVECTOR(0) };
	TRANS leftArmTrans_upperarm = { leftArmTrans.x + CALVECTOR(40) , leftArmTrans.z + CALVECTOR(0) , -leftArmTrans.y + CALVECTOR(-320) };
	TRANS leftArmTrans_shoulder = { leftArmTrans.x + CALVECTOR(-20) , leftArmTrans.y + CALVECTOR(290) , leftArmTrans.z + CALVECTOR(0) };

	ROTATE leftArmRotate_upperarm = { 90, 1, 0, 0 };

	drawHalfSphere(leftArmTrans_shoulder, CALVECTOR(40), 1);
	drawCylinder(leftArmColor, leftArmTrans_upperarm, leftArmRotate_upperarm, CALVECTOR(20), CALVECTOR(100),6);
	drawSphere(leftArmColor, leftArmTrans_hand, CALVECTOR(20),6);
};

void drawRightArm() {
	RGB rightArmColor = { CALRGB(255), CALRGB(220), CALRGB(177) };
	TRANS rightArmTrans = { CALVECTOR(-250) , CALVECTOR(-350) , CALVECTOR(0) };

	TRANS rightArmTrans_hand = { rightArmTrans.x + CALVECTOR(125) , rightArmTrans.y + CALVECTOR(270) , rightArmTrans.z + CALVECTOR(0) };
	TRANS rightArmTrans_upperarm = { rightArmTrans.x + CALVECTOR(125) , rightArmTrans.z + CALVECTOR(0) , -rightArmTrans.y + CALVECTOR(-320) };
	TRANS rightArmTrans_shoulder = { rightArmTrans.x + CALVECTOR(20) , rightArmTrans.y + CALVECTOR(290) , rightArmTrans.z + CALVECTOR(0) };

	ROTATE rightArmRotate_upperarm = { 90, 1, 0, 0 };

	drawHalfSphere(rightArmTrans_shoulder, CALVECTOR(40), 1);
	drawCylinder(rightArmColor, rightArmTrans_upperarm, rightArmRotate_upperarm, CALVECTOR(20), CALVECTOR(100),6);
	drawSphere(rightArmColor, rightArmTrans_hand, CALVECTOR(20),6);
};

void drawTrunk() {
	RGB trunkColor = { CALRGB(255), CALRGB(220), CALRGB(177) };

	TRANS trunkTrans_1 = { CALVECTOR(-165) , CALVECTOR(-20) , CALVECTOR(0) };
	TRANS trunkTrans_2 = { CALVECTOR(-165) , CALVECTOR(-30) , CALVECTOR(0) };
	TRANS trunkTrans_3 = { CALVECTOR(-165) , CALVECTOR(-40) , CALVECTOR(0) };
	TRANS trunkTrans_4 = { CALVECTOR(-165) , CALVECTOR(-50) , CALVECTOR(0) };
	TRANS trunkTrans_5 = { CALVECTOR(-165) , CALVECTOR(-60) , CALVECTOR(0) };
	TRANS trunkTrans_6 = { CALVECTOR(-165) , CALVECTOR(-70) , CALVECTOR(0) };
	TRANS trunkTrans_7 = { CALVECTOR(-165) , CALVECTOR(-80) , CALVECTOR(0) };
	TRANS trunkTrans_8 = { CALVECTOR(-165) , CALVECTOR(-90) , CALVECTOR(0) };
	TRANS trunkTrans_9 = { CALVECTOR(-165) , CALVECTOR(-100) , CALVECTOR(0) };
	TRANS trunkTrans_10 = { CALVECTOR(-165) , CALVECTOR(-110) , CALVECTOR(0) };
	TRANS trunkTrans_11 = { CALVECTOR(-165) , CALVECTOR(-120) , CALVECTOR(0) };
	TRANS trunkTrans_12 = { CALVECTOR(-165) , CALVECTOR(-130) , CALVECTOR(0) };
	TRANS trunkTrans_13 = { CALVECTOR(-165) , CALVECTOR(-140) , CALVECTOR(0) };
	TRANS trunkTrans_14 = { CALVECTOR(-165) , CALVECTOR(-150) , CALVECTOR(0) };

	ROTATE trunkRotate = { 90,1,0,0 };

	drawTorus(trunkColor, trunkTrans_1, 5, 20, trunkRotate, 10, 9, 11, 9);
	drawTorus(trunkColor, trunkTrans_2, 5, 20, trunkRotate, 8, 10, 10, 10);
	drawTorus(trunkColor, trunkTrans_3, 5, 20, trunkRotate, 10, 9, 9, 9);
	drawTorus(trunkColor, trunkTrans_4, 5, 20, trunkRotate, 8, 8, 8, 8);
	drawTorus(trunkColor, trunkTrans_5, 5, 20, trunkRotate, 10, 7, 8, 7);
	drawTorus(trunkColor, trunkTrans_6, 5, 20, trunkRotate, 8, 7, 8, 7);
	drawTorus(trunkColor, trunkTrans_7, 5, 20, trunkRotate, 10, 7, 8, 7);
	drawTorus(trunkColor, trunkTrans_8, 5, 20, trunkRotate, 8, 7, 8, 7);
	drawTorus(trunkColor, trunkTrans_9, 5, 20, trunkRotate, 10, 7, 8, 7);
	drawTorus(trunkColor, trunkTrans_10, 5, 20, trunkRotate, 8, 7, 8, 7);
	drawTorus(trunkColor, trunkTrans_11, 5, 20, trunkRotate, 10, 7, 8, 7);
	drawTorus(trunkColor, trunkTrans_12, 5, 20, trunkRotate, 8, 7, 8, 7);
	drawTorus(trunkColor, trunkTrans_13, 5, 20, trunkRotate, 10, 7, 8, 7);
	drawTorus(trunkColor, trunkTrans_14, 5, 20, trunkRotate, 8, 7, 8, 7);

};

void drawLeftLeg() {

	RGB leftLegColor = { CALRGB(255), CALRGB(220), CALRGB(177) };
	TRANS leftLegTrans = { CALVECTOR(-130) , CALVECTOR(-400) , CALVECTOR(0) };

	TRANS leftLegTrans_footEnd = { leftLegTrans.x + CALVECTOR(50) , leftLegTrans.y + CALVECTOR(50) , leftLegTrans.z + CALVECTOR(0) };
	TRANS leftLegTrans_foot = { leftLegTrans.x + CALVECTOR(20) , leftLegTrans.y + CALVECTOR(50) , leftLegTrans.z + CALVECTOR(0) };

	TRANS leftLegTrans_ankleJoint = { leftLegTrans.x + CALVECTOR(65) , leftLegTrans.y + CALVECTOR(240) , leftLegTrans.z + CALVECTOR(0) };
	TRANS leftLegTrans_calf = { leftLegTrans.x + CALVECTOR(65) , leftLegTrans.z + CALVECTOR(0) , -leftLegTrans.y + CALVECTOR(-300) };
	TRANS leftLegTrans_pant = { leftLegTrans.x + CALVECTOR(65) , leftLegTrans.z + CALVECTOR(0) , -leftLegTrans.y + CALVECTOR(-340) };

	SCALA leftLegScala_foot = { CALVECTOR(400) , CALVECTOR(130) , CALVECTOR(180) };

	ROTATE leftLegRotate_foot = { 0, 0, 0, 0 };
	ROTATE leftLegRotate_calf_thigh = { 90, 1, 0, 0 };

	RGB leftLegColor_foot = { CALRGB(255), CALRGB(0), CALRGB(0) };

	drawCylinder(leftLegColor, leftLegTrans_pant, leftLegRotate_calf_thigh, CALVECTOR(35), CALVECTOR(100),11);
	drawCylinder(leftLegColor, leftLegTrans_calf, leftLegRotate_calf_thigh, CALVECTOR(20), CALVECTOR(120),6);
	drawSphere(leftLegColor, leftLegTrans_ankleJoint, CALVECTOR(20),6);
	drawCube(leftLegColor_foot, leftLegTrans_foot, leftLegScala_foot, leftLegRotate_foot, 2);
	drawShoe(leftLegTrans_foot, 15);
};

void drawRightLeg() {

	RGB rightLegColor = { CALRGB(255), CALRGB(220), CALRGB(177) };
	TRANS rightLegTrans = { CALVECTOR(-200) , CALVECTOR(-400) , CALVECTOR(0) };

	TRANS rightLegTrans_footEnd = { rightLegTrans.x + CALVECTOR(50) , rightLegTrans.y + CALVECTOR(50) , rightLegTrans.z + CALVECTOR(0) };
	TRANS rightLegTrans_foot = { rightLegTrans.x + CALVECTOR(-20) , rightLegTrans.y + CALVECTOR(50) , rightLegTrans.z + CALVECTOR(0) };

	TRANS rightLegTrans_ankleJoint = { rightLegTrans.x + CALVECTOR(100) , rightLegTrans.y + CALVECTOR(240) , rightLegTrans.z + CALVECTOR(0) };
	TRANS rightLegTrans_calf = { rightLegTrans.x + CALVECTOR(100) , rightLegTrans.z + CALVECTOR(0) , -rightLegTrans.y + CALVECTOR(-300) };
	TRANS rightLegTrans_pant = { rightLegTrans.x + CALVECTOR(100) , rightLegTrans.z + CALVECTOR(0) , -rightLegTrans.y + CALVECTOR(-340) };

	SCALA rightLegScala_foot = { CALVECTOR(400) , CALVECTOR(130) , CALVECTOR(180) };

	ROTATE rightLegRotate_foot = { 0, 0, 0, 0 };
	ROTATE rightLegRotate_calf_thigh = { 90, 1, 0, 0 };

	RGB rightLegColor_foot = { CALRGB(255), CALRGB(0), CALRGB(0) };

	drawCylinder(rightLegColor, rightLegTrans_pant, rightLegRotate_calf_thigh, CALVECTOR(35), CALVECTOR(100),11);
	drawCylinder(rightLegColor, rightLegTrans_calf, rightLegRotate_calf_thigh, CALVECTOR(20), CALVECTOR(120),6);
	drawSphere(rightLegColor, rightLegTrans_ankleJoint, CALVECTOR(20), 6);
	drawCube(rightLegColor_foot, rightLegTrans_foot, rightLegScala_foot, rightLegRotate_foot, 2);
	drawShoe(rightLegTrans_foot, 15);
};

void drawBody() {
	drawLeftArm();
	drawRightArm();
	drawTrunk();
	drawLeftLeg();
	drawRightLeg();
};
 
GLfloat sunREL[6][2] = { {CALVECTOR(0),CALVECTOR(80)},{CALVECTOR(-80),CALVECTOR(40)},{CALVECTOR(-80),CALVECTOR(-40)},
{ CALVECTOR(0),CALVECTOR(-80)},{CALVECTOR(80),CALVECTOR(-40)},{CALVECTOR(80),CALVECTOR(40)}};

int sunREL1 = 0;
int sunREL2 = 1;
int sunREL3 = 2;
int sunREL4 = 3;
int sunREL5 = 4;
int sunREL6 = 5;

void updateSunTrans(int direction) { 
	sunREL1 = (sunREL1 + direction) % 6;
	sunREL2 = (sunREL2 + direction) % 6;
	sunREL3 = (sunREL3 + direction) % 6;
	sunREL4 = (sunREL4 + direction) % 6;
	sunREL5 = (sunREL5 + direction) % 6;
	sunREL6 = (sunREL6 + direction) % 6;
}

void drawSun() {
	
	RGB sunColor = { CALRGB(122), CALRGB(0), CALRGB(122) };
	TRANS sunTrans = { CALVECTOR(100) , CALVECTOR(200) , CALVECTOR(0) };
	TRANS flowTrans = { CALVECTOR(0) , CALVECTOR(-860) , CALVECTOR(0) };

	TRANS sunTrans_Center = { sunTrans.x - CALVECTOR(50), sunTrans.y - CALVECTOR(100) , sunTrans.z + CALVECTOR(0) };
	TRANS sunTrans_REL1 = { sunTrans.x + sunREL[sunREL1][0] , sunTrans.y + sunREL[sunREL1][1] , sunTrans.z + CALVECTOR(0) };
	TRANS sunTrans_REL2 = { sunTrans.x + sunREL[sunREL2][0] , sunTrans.y + sunREL[sunREL2][1] , sunTrans.z + CALVECTOR(0) };
	TRANS sunTrans_REL3 = { sunTrans.x + sunREL[sunREL3][0] , sunTrans.y + sunREL[sunREL3][1] , sunTrans.z + CALVECTOR(0) };
	TRANS sunTrans_REL4 = { sunTrans.x + sunREL[sunREL4][0] , sunTrans.y + sunREL[sunREL4][1] , sunTrans.z + CALVECTOR(0) };
	TRANS sunTrans_REL5 = { sunTrans.x + sunREL[sunREL5][0] , sunTrans.y + sunREL[sunREL5][1] , sunTrans.z + CALVECTOR(0) };
	TRANS sunTrans_REL6 = { sunTrans.x + sunREL[sunREL6][0] , sunTrans.y + sunREL[sunREL6][1] , sunTrans.z + CALVECTOR(0) };

	SCALA sunScala_REL = { CALVECTOR(100) , CALVECTOR(100) , CALVECTOR(100) };

	ROTATE sunRotate_REL1 = { 90, -2, 1, 0 };
	ROTATE sunRotate_REL2 = { 90, -2, 1, 0 };
	ROTATE sunRotate_REL3 = { 90, -2, 1, 0 };
	ROTATE sunRotate_REL4 = { 90, -2, 1, 0 };
	ROTATE sunRotate_REL5 = { 90, -2, 1, 0 };
	ROTATE sunRotate_REL6 = { 90, -2, 1, 0 };

	drawSphere(sunColor, sunTrans_Center, CALVECTOR(60), 13);
	drawCube(sunColor, sunTrans_REL1, sunScala_REL, sunRotate_REL1, 0);
	drawCube(sunColor, sunTrans_REL2, sunScala_REL, sunRotate_REL2, 1);
	drawCube(sunColor, sunTrans_REL3, sunScala_REL, sunRotate_REL3, 2);
	drawCube(sunColor, sunTrans_REL4, sunScala_REL, sunRotate_REL4, 3);
	drawCube(sunColor, sunTrans_REL5, sunScala_REL, sunRotate_REL5, 4);
	drawCube(sunColor, sunTrans_REL6, sunScala_REL, sunRotate_REL6, 5);
	drawRectangle(flowTrans,14);
};

void drawHuman() {
	
	glClearColor(0.0, 0.7, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	cameraX = radius * cos(phi) * cos(theta);
	cameraY = radius * cos(phi) * sin(theta);
	cameraZ = radius * sin(phi);


	gluLookAt(cameraX, cameraY, cameraZ, 0, 0, 0.0, 0.0, 1.0, 0.0);


	glRotatef(cameraRotationX, 1.0f, 0.0f, 0.0f);
	glRotatef(cameraRotationY, 0.0f, 1.0f, 0.0f);

	drawHead();
	drawBody();
	drawSun();
	
	glutSwapBuffers();

}

void moveMousePoint(char* format, ...)
{
	va_list args;
	va_start(args, format);
	va_end(args);
}

POINT mouseDownPt;

void mouseCallback(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		moveMousePoint("[Mouse Down] ");
		mouseDownPt.x = x;
		mouseDownPt.y = y;
	}
	else if (state == GLUT_UP)
		moveMousePoint("[Mouse Up] ");
	if (button == GLUT_LEFT_BUTTON)
		moveMousePoint("Left Button ");
	else if (button == GLUT_MIDDLE_BUTTON)
		moveMousePoint("Middle Button ");
	else if (button == GLUT_RIGHT_BUTTON)
		moveMousePoint("Right Button ");
	moveMousePoint("( %d, %d )\n", x, y);
	// 마우스 드래그로 시점 회전시키기
}
void motionCallback(int x, int y)
{
	moveMousePoint("[Mouse Draged] ( %d, %d )\n", x, y);
	int X = x;
	int Y = y;

	cameraRotationX += (Y - mouseDownPt.y) / 5.6;
	cameraRotationY += (X - mouseDownPt.x) / 5.6;

	mouseDownPt.x = X;
	mouseDownPt.y = Y;
}
void passiveMotionCallback(int x, int y)
{

}

int main(int argc, char** argv) {


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("OpenGL Project");

	glInit();
	
	glutDisplayFunc(drawHuman);
	glutIdleFunc(update);

	glutReshapeFunc(resizeWindow);
	glutSpecialFunc(keyEventHandler);
	glutKeyboardFunc(processNormalKeys);

	glutMouseFunc(mouseCallback);
	glutMotionFunc(motionCallback);
	glutPassiveMotionFunc(passiveMotionCallback);

	glutMainLoop();

	return 0;
}

void resizeWindow(int width, int height)
{
	if (height == ZERO)
		height = INT_ONE;

	float ratio = FlOAT_ONE * width / height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(35, ratio, INT_ONE, 500);
	glMatrixMode(GL_MODELVIEW);
	glViewport(ZERO, ZERO, width, height);

}

void glInit() {
	
	LoadGLColorTextures();
	LoadGLImageTextures();
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);

	//InitLight();
}

void InitLight() {
	GLfloat mat_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
	GLfloat mat_shininess[] = { 15.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 0.99, 0.99, 0.99, 1.0 };
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light_position[] = { -3, 6, 3.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void keyEventHandler(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_RIGHT:      
		phi += 0.05;
		break;
	case GLUT_KEY_LEFT:    
		phi -= 0.05;
		break;
	case GLUT_KEY_UP:
		updateSunTrans(1);
		break;
	case GLUT_KEY_DOWN:
		updateSunTrans(5);
		break;
	default:               
		break;
	}
}