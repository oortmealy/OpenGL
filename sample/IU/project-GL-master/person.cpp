#include <Windows.h>
#include <math.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/GLAUX.H>
#include <GL/GL.h>
#define TEX_BLACK 1
#define TEX_BODY 2
#define TEX_SKIN 3
#define TEX_ARM 4
#define TEX_SHOE 5
#define TEX_EYE 6
#define TEX_FACE 7
#define TEX_HAIR 8
#define PI 3.141592
void createCircle(GLfloat, GLfloat, GLfloat);
void createCylinder(GLfloat,GLfloat,GLfloat);
void createCylinder_tex(GLfloat,GLfloat,GLfloat, int, int);
int LoadGLTextures(char*,int);
void createSphere(GLfloat);
AUX_RGBImageRec *LoadBMPFile(char *Filename);
float x, y, z;
float radius;
float theta;
float phi;
float zoom = 60.0;

int beforeX, beforeY;

GLuint texture[10];                          // 텍스처 저장공간

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);                // WndProc 선언

void InitLight()
{
	GLfloat ambientLight[4];
    GLfloat diffuseLight[4];
    GLfloat lightPosition[4];
    GLfloat specular[4];
    GLfloat yrot;

	ambientLight[0] = 0.3f; 
    ambientLight[1] = 0.3f;
    ambientLight[2] = 0.3f;
    ambientLight[3] = 1.0f;

    diffuseLight[0] = 0.7f;
    diffuseLight[1] = 0.7f;
    diffuseLight[2] = 0.7f;
    diffuseLight[3] = 1.0f;

    lightPosition[0] = 0.0f;
    lightPosition[1] = 0.0f;
    lightPosition[2] = 5.0f;
    lightPosition[3] = 1.0f;

    specular[0] = 1.0f;
    specular[1] = 1.0f;
    specular[2] = 1.0f;
    specular[3] = 1.0f;

    yrot = 0.0f;

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f); //밝은 회색을 배경색으로 설정 
    glFrontFace(GL_CCW); //반시계방향으로 감은 폴리곤이 앞면

    glEnable(GL_LIGHTING); //조명 켜기
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight); //설정
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight); //설정
    //glLightfv(GL_LIGHT0, GL_SPECULAR, specular); //설정
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); //설정
    glEnable(GL_LIGHT0); // 0번 조명 사용
}

int init (void)
{
	// texture loading
	LoadGLTextures("Data/black.bmp",TEX_BLACK);
	LoadGLTextures("Data/body.bmp",TEX_BODY);
	LoadGLTextures("Data/skin.bmp",TEX_SKIN);
	LoadGLTextures("Data/arm.bmp",TEX_ARM);
	LoadGLTextures("Data/shoe.bmp",TEX_SHOE);
	LoadGLTextures("Data/eye.bmp",TEX_EYE);
	LoadGLTextures("Data/face.bmp",TEX_FACE);
	LoadGLTextures("Data/hair.bmp",TEX_HAIR);

	glEnable(GL_TEXTURE_2D);                        // 텍스처 매핑을 활성화시킴 ( 새코드 )
	glShadeModel(GL_SMOOTH);                        // 부드러운 쉐이딩을 활성화시킴
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                    // 검은색 배경
    glClearDepth(1.0f);                            // 깊이버퍼 설정
    glEnable(GL_DEPTH_TEST);                        // 깊이테스트를 켬
    glDepthFunc(GL_LEQUAL);                            // 깊이테스트 종류
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);            // 원근 계산
    return TRUE;                                // 초기화 성공
}

void reshape (int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
//    glOrtho (-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);
    gluPerspective(zoom, 1.0, 1.0, 100.0);  // 멀고 가까움을 표현.

    radius = 10.0;
    theta = 10.0;
    phi = -10.0;
}
void display (void)
{
    // for zoom
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(zoom, 1.0, 1.0, 100.0);  // 멀고 가까움을 표현.

    x = -radius * cos(phi) * cos(theta);
    y = -radius * cos(phi) * sin(theta);
    z = radius * sin(phi);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	
	// modeling & texure mapping
    glBindTexture(GL_TEXTURE_2D, texture[TEX_ARM]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(2.5, 0.0, -0.5);
	glRotatef(30, 0.0, -1.0, 0.0);
    createCylinder_tex(0.4, 0.4, 2.0,TEX_SKIN,TEX_BLACK); // right arm
	
    glBindTexture(GL_TEXTURE_2D, texture[TEX_ARM]);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.5, -2.0, 1.0);
	glRotatef(90.0, -1.0, 0.0, 0.0);
    createCylinder_tex(0.4, 0.4, 2.0,TEX_SKIN,TEX_BLACK); // left arm
	
    glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-0.8, 0.0, -3.2);
    createCylinder(0.4, 1.0, 3.0); // ieft leg
	
    glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.8, 0.0, -3.2);
    createCylinder(0.4, 1.0, 3.0); // right leg

    glBindTexture(GL_TEXTURE_2D, texture[TEX_BODY]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.0, -1.5);
	createCylinder_tex(2.3,1.3,3.0,TEX_BLACK,TEX_BLACK);   // body

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.5, -2.0, 1.0);
    createSphere(0.4f); // left hand

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.4, -2.2, 1.2);
    createCylinder(0.1, 0.1, 0.4); // left finger

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.4, -2.2, 1.6);
    createSphere(0.1f); // left finger point

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.05, -2.2, 1.0);
	glRotatef(90.0, 0.0, -1.0, 0.0);
    createCylinder(0.1, 0.1, 0.4); // left finger2
	
	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.05, -2.2, 1.0);
    createSphere(0.1f); // left finger2 point

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(2.5, 0.0, -0.5);
    createSphere(0.4f); // right hand
	
	glBindTexture(GL_TEXTURE_2D, texture[TEX_BLACK]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.5, 0.0, 1.0);
    createSphere(0.4f); // left sholder

	glBindTexture(GL_TEXTURE_2D, texture[TEX_BLACK]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(1.5, 0.0, 1.2);
    createSphere(0.4f); // right sholder

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SHOE]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.8, 0.0, -3.2);
    createSphere(0.4f); // right shoe
	
	glBindTexture(GL_TEXTURE_2D, texture[TEX_SHOE]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-0.8, 0.0, -3.2);
    createSphere(0.4f); // left shoe

	glBindTexture(GL_TEXTURE_2D, texture[TEX_HAIR]);			
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-0.5, -0.1, 3.6);
	glRotatef(50, 0.0, -1.0, 0.0);
	glScalef(1.0, 1.0, 0.8);
    createSphere(1.8f); // left hair
	
	glBindTexture(GL_TEXTURE_2D, texture[TEX_HAIR]);			
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.5, -0.1, 3.6);
	glRotatef(-50, 0.0, -1.0, 0.0);
	glScalef(1.0, 1.0, 0.8);
    createSphere(1.8f); // right hair
	
	glBindTexture(GL_TEXTURE_2D, texture[TEX_FACE]);			
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.0, 3.0);
	glScalef(0.9, 1.0, 1.0);
    createSphere(2.0f); // head

	glBindTexture(GL_TEXTURE_2D, texture[TEX_HAIR]);			
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.3, 3.2);
	glRotatef(-180, 0.0, 0.0, 1.0);
    createSphere(2.0f); // back hair

	glBindTexture(GL_TEXTURE_2D, texture[TEX_HAIR]);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(2.0, 0.6, 2.2);
	glRotatef(-60, 0.0, -1.0, 0.0);
	glScalef(4.0, 1.2, 1.3);
    createSphere(0.4f); // right pony

	glBindTexture(GL_TEXTURE_2D, texture[TEX_HAIR]);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-2.0, 0.6, 2.2);
	glRotatef(60, 0.0, -1.0, 0.0);
	glScalef(4.0, 1.2, 1.3);
    createSphere(0.4f); // left pony
	
    glFlush ();
    glutSwapBuffers();
}

// ZOOM
void keyboard( unsigned char key, int x, int y )
{
    switch(key)
    {
        case 'a':
            zoom--;
            break;
        case 'z':
            zoom++;
            break;
    }

}

// VIEWPOINT 1
void processMouse(int button, int state, int x, int y)
{

    if(button == GLUT_LEFT_BUTTON){
        if(state == GLUT_DOWN){
            beforeX = x;
            beforeY = y;
        }
    }
}

// VIEWPOINT 2
void processMouseMotion(int x, int y)
{
    if(abs(beforeX-x) > abs(beforeY-y)){
        if(beforeX < x)
        {
            theta -= 0.1;
        }else if(beforeX > x){
            theta += 0.1;
        }
    }else {
        if(beforeY > y){
            phi -= 0.1;
        }else if(beforeY < y){
            phi -= 0.1;
        }
    }

    beforeX = x;
    beforeY = y;

    glutPostRedisplay();

    if ( theta > 2.0 * PI ) // 360도 넘어가면
        theta -= (2.0 * PI);
    else if ( theta < 0.0 )
        theta += (2.0 * PI);
}

void createSphere(GLfloat r)
{
GLUquadricObj *sphere = gluNewQuadric();
  
  gluQuadricTexture(sphere, GL_TRUE); 
  gluQuadricDrawStyle(sphere, GLU_FILL); 
  glPolygonMode(GL_FRONT, GL_FILL); 
  gluQuadricNormals(sphere, GLU_SMOOTH);
  gluSphere(sphere,r,36 ,18);
}

void createCylinder_tex(GLfloat bottom, GLfloat top, GLfloat height, int texture1, int texture2)
{
  GLUquadricObj *cylinder = gluNewQuadric();
  
  gluQuadricTexture(cylinder, GL_TRUE); 
  gluQuadricDrawStyle(cylinder, GLU_FILL); 
  glPolygonMode(GL_FRONT, GL_FILL); 
  gluQuadricNormals(cylinder, GLU_SMOOTH);
  gluCylinder(cylinder, bottom, top, height, 20, 100);
  
  glBindTexture(GL_TEXTURE_2D, texture[texture1]);
  createCircle(bottom, 1.0f, 0.0f); // 원기둥 밑면
  glBindTexture(GL_TEXTURE_2D, texture[texture2]);
  createCircle(top, -1.0f, height); // 원기둥 윗면
}

void createCylinder(GLfloat bottom, GLfloat top, GLfloat height)
{
  GLUquadricObj *cylinder = gluNewQuadric();
  
  gluQuadricTexture(cylinder, GL_TRUE); 
  gluQuadricDrawStyle(cylinder, GLU_FILL); 
  glPolygonMode(GL_FRONT, GL_FILL); 
  gluQuadricNormals(cylinder, GLU_SMOOTH);
  gluCylinder(cylinder, bottom, top, height, 20, 100);
  
  createCircle(bottom, 1.0f, 0.0f); // 원기둥 밑면
  createCircle(top, -1.0f, height); // 원기둥 윗면
}

// for cylinder top & bottom
void createCircle(GLfloat r, GLfloat pos, GLfloat move){
  GLfloat centerx = 0, centery = 0, centerz = 0;
  GLfloat x, y, angle;

  glBegin(GL_TRIANGLE_FAN);
  glTexCoord2f(0.0f, 0.0f);
  glNormal3f(0.0f, 0.0f, pos);
  glVertex3f(centerx, centery, centerz + move);
  for(angle = (2.0f*PI); angle > 0.0f; angle -= (PI/8.0f))
  {
      x = centerx + r*sin(angle);
      y = centery + r*cos(angle);
	  glTexCoord2f(x, y);
      glNormal3f(0.0f, 0.0f, pos);
      glVertex3f(x, y, centerz + move);
  }
  glEnd();
}

// texure load 1
AUX_RGBImageRec *LoadBMPFile(char *Filename)
{
	FILE *File=NULL;                            // 파일 핸들
	if (!Filename)                                // 파일이름이 전달되었는지 확인
   {
       return NULL;                            // 그렇지 않다면 NULL을 반환
   }

	File=fopen(Filename,"r");                        // 파일이 존재하는지 확인
	if (File)                                // 파일이 존재하는가?
   {
       fclose(File);                            // 핸들을 닫음
       return auxDIBImageLoad(Filename);                // 비트맵을 읽어들이고 포인터를 반환
   }

	return NULL;
}

// texure load 2
int LoadGLTextures(char* filename, int num)
{
	int Status=FALSE;                            // 상태 표시기
	AUX_RGBImageRec *TextureImage[1];                    // 텍스처용 저장공간을 만듬
	memset(TextureImage,0,sizeof(void *)*1);                // 포인터를 NULL로 설정

	//printf("%s\n",filename);
	if (TextureImage[0]=LoadBMPFile(filename))
	{
       Status=TRUE;                            // Status를 TRUE로 설정
	   glGenTextures(1, &texture[num]);                    // 텍스처를 만듬

       // 비트맵으로부터 가져온 데이터를 사용한 일반적인 텍스처 생성
       glBindTexture(GL_TEXTURE_2D, texture[num]);
	   // 텍스처를 만든다
       glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);    // 선형 필터링
       glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);    // 선형 필터링
	}

	 if (TextureImage[0])                            // 텍스처가 존재하는지 확인
   {
       if (TextureImage[0]->data)                    // 텍스처 이미지가 존재하는지 확인
       {
           free(TextureImage[0]->data);                // 텍스처 이미지 메모리를 해제
       }

       free(TextureImage[0]);                        // 이미지 구조체를 해제
   }

	return Status;                                // Status를 반환
}

void main (int argc, char** argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowPosition (100, 100);
    glutInitWindowSize (1000, 1000);
    glutCreateWindow ("IU YOU&I ver");
    init();
    InitLight();
    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    glutMouseFunc(processMouse);
    glutMotionFunc(processMouseMotion);
	glutKeyboardFunc(keyboard);
    glutIdleFunc(display);
    glutMainLoop();
}