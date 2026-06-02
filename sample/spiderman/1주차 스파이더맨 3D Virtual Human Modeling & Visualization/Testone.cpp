#include <glut.h>
#include <gl.h>
#include <glu.h>
#include <math.h>
#pragma warning(disable:4996)
// 텍스쳐 매핑에 필요한 헤더
#include <stdio.h> 
#include <glaux.h>

#define PI_ 3.14159265358979323846 
#define NULL 0

GLuint theTorus; /* Draw a torus */
typedef float vec3_t[3];


// 텍스쳐 저장용 공간, 3개의 텍스쳐(1=몸통, 2=등, 3=머리)
GLuint texture[3]; 

// bmp이미지를 읽는 메소드
AUX_RGBImageRec *LoadBmpImage(char *Filename){ 
	FILE *File = NULL; // 파일 핸들
	if(!Filename) return NULL; //실제 파일 이름이 주어졌는지 확인
	File = fopen(Filename, "r"); // 파일이 존재하는지 확인, 성공하면 존재 
	
	if(File){
		fclose(File); // 파일이 열렸다면 파일 존재 확인했으니 다시 닫아줌
		return auxDIBImageLoad(Filename); // 비트맵을 읽어들이고 포인터를 반환
	}
	return NULL;
}

//텍스쳐 맵핑, bmp를 로드하고 texture로 변환
int LoadGLTextures(){ 
	int Status=FALSE; // 성공 여부 상태 표시를 위한 변수, 최초는 아무것도 만들거나 읽지 않아서 false

	AUX_RGBImageRec *TextureImage[3]; // textuer 저장공간 생성, bmp 폭과 높이 및 데이터 저장
	memset(TextureImage,0,sizeof(void *)*1); // 이미지 레코드를 지움, 포인터=NULL

	// bmp를 로드(3개)
	if ((TextureImage[0]=LoadBmpImage("image/front.bmp")) && (TextureImage[1]=LoadBmpImage("image/back.bmp"))
	    && (TextureImage[2]=LoadBmpImage("image/head.bmp")))
	{
       Status=TRUE;                     // Status를 TRUE로 설정
	   glGenTextures(3, &texture[0]);   // texture 3개 생성

	   for(int i=0; i<3; i++) { // texture 생성, 값은 자동으로 채워 넣는다.
		   glBindTexture(GL_TEXTURE_2D, texture[i]);
		   glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[i]->sizeX, TextureImage[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,TextureImage[i]->data);
		   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);    // 이미지가 원래 txture 크기보다 작게 나타날때 -> 선형 필터링
		   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);    // 이미지가 원래 txture 크기보다 크게 나타날때 -> 선형 필터링
	   }
	} else return FALSE;

	for(int i=0; i<3; i++) {
	   if (TextureImage[i]){                  // texture 존재 확인
		   if (TextureImage[i]->data){        // texture 이미지가 존재하는지 확인
			    free(TextureImage[i]->data);  // texture 이미지 메모리를 해제
		   }

		   free(TextureImage[i]);             // 이미지 구조체를 해제
	   }else return FALSE;
	} 

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	return Status;	// status를 반환
}

vec3_t color[] = {
	{ 1.0f, 0.0f, 0.0f }, //red
	{ 0.0f, 1.0f, 0.0f }, //green
	{ 0.0f, 0.0f, 1.0f }, //blue
	{ 1.0f, 1.0f, 0.0f }, //yellow
	{ 1.0f, 0.0f, 1.0f }, //magenta
	{ 0.0f, 1.0f, 1.0f }, //cyan
	{ 1.0f, 1.0f, 1.0f }, //white
	{ .25f, .25f, .25f }, //dark gray
	{ .60f, .40f, .70f }, //barney purple
	{ .98f, .625f, .12f }, //pumpkin orange
	{ .98f, .04f, .70f }, //pastel pink
	{ .75f, .75f, .75f }, //light gray
	{ .60f, .40f, .12f } //brown
};
vec3_t rot = { 0., 0., 0. };
vec3_t eye = { 0., 0., 0.0001 }; // -5.0에서 0.0001로 수정
vec3_t center = { 0., 0., 0. };

float size = 3.;
float theta = .0;
float thetaDelta = .125;
float eyeDelta = .125;
float scale = 0.6;
float scaleDelta = 1.05;//1.125;
float anitheta = 0;
float anithetaDelta = .125;
GLUquadricObj *c;

int mouseX = 0;
int mouseY = 0;
int mouseState = 0;
int mouseButton = 0;
int projection = 0;
int aniOn = 0;
int depthOn = 1;
int openOn = 0;
int fillOn = 0;
int windowWidth, windowHeight;


static void torus(int numc, int numt)
{
	int i, j, k;
	double s, t, x, y, z, twopi;
	twopi = 2 * PI_;

	float vec3_t[3] = { 0.0f, 0.1f, 1.0f };
	glColor3fv(vec3_t);

	for (i = 0; i < numc; i++) {
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= (numt * 1) / 5; j++) {		//팔 = 1/5,		손 = 3/4
			for (k = 1; k >= 0; k--) {
				s = (i + k) % numc + 0.5;
				t = j % numt;
				x = (1 + .2*cos(s*twopi / numc))*cos(t*twopi / numt);
				y = (1 + .2*cos(s*twopi / numc))*sin(t*twopi / numt);
				z = .3 * sin(s * twopi / numc);
				glVertex3f(x, y, z);
			}
		}
		glEnd();
	}
}
static void torus_hand(int numc, int numt)
{
	int i, j, k;
	double s, t, x, y, z, twopi;
	twopi = 2 * PI_;

	float vec3_t[3] = { 1.0f, 0.0f, 0.0f };
	glColor3fv(vec3_t);

	for (i = 0; i < numc; i++) {
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= (numt * 3) / 4; j++) {		//팔 = 1/4,		손 = 3/4
			for (k = 1; k >= 0; k--) {
				s = (i + k) % numc + 0.5;
				t = j % numt;
				x = (1 + .3*cos(s*twopi / numc))*cos(t*twopi / numt);
				y = (1 + .3*cos(s*twopi / numc))*sin(t*twopi / numt);
				z = .3 * sin(s * twopi / numc);
				glVertex3f(x, y, z);
			}
		}
		glEnd();
	}
}
static void torus_hat(int numc, int numt)
{
	int i, j, k;
	double s, t, x, y, z, twopi;
	twopi = 2 * PI_;

	float vec3_t[3] = { 1.0f, 0.0f, 0.0f };
	glColor3fv(vec3_t);

	for (i = 0; i < numc; i++) {
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= numt; j++) {		//팔 = 1/4,		손 = 3/4
			for (k = 1; k >= 0; k--) {
				s = (i + k) % numc + 0.5;
				t = j % numt;
				x = (1 + .1*cos(s*twopi / numc))*cos(t*twopi / numt);
				y = (1 + .1*cos(s*twopi / numc))*sin(t*twopi / numt);
				z = .4 * sin(s * twopi / numc);
				glVertex3f(x, y, z);
			}
		}
		glEnd();
	}
}

static void Leg()
{
	glBegin(GL_QUADS);              
	// Top face
	glColor3f(0.0f, 0.0f, 1.0f);   
	glVertex3f(-1.0f, -1.0f, 0.5f);
	glVertex3f(-1.0f, -1.0f, -0.7f);
	glVertex3f(-0.1f, -1.0f, -0.7f);
	glVertex3f(-0.1f, -1.0f, 0.5f);

	// Bottom face
	glColor3f(0.0f, 0.0f, 1.0f);  
	glVertex3f(-1.0f, -3.0f, 0.5f);
	glVertex3f(-1.0f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, 0.5f);

	// Front face 
	glColor3f(0.0f, 0.0f, 1.0f);  
	glVertex3f(-1.0f, -1.0f, 0.5f);
	glVertex3f(-0.1f, -1.0f, 0.5f);
	glVertex3f(-0.1f, -3.0f, 0.5f);
	glVertex3f(-1.0f, -3.0f, 0.5f);

	// Back face
	glColor3f(0.0f, 0.0f, 1.0f);   
	glVertex3f(-1.0f, -1.0f, -0.7f);
	glVertex3f(-0.1f, -1.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-1.0f, -3.0f, -0.7f);

	// Left face
	glColor3f(0.0f, 0.0f, 1.0f);   
	glVertex3f(-1.0f, -1.0f, 0.5f);
	glVertex3f(-1.0f, -1.0f, -0.7f);
	glVertex3f(-1.0f, -3.0f, -0.7f);
	glVertex3f(-1.0f, -3.0f, 0.5f);

	// Right face
	glColor3f(0.0f, 0.0f, 1.0f);  
	glVertex3f(-0.1f, -1.0f, 0.5f);
	glVertex3f(-0.1f, -1.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, 0.5f);
	glEnd(); 
}

static void foot()
{
	glBegin(GL_QUADS);           
	// Top face 
	glColor3f(0.2f, 0.2f, 0.8f);  
	glVertex3f(-1.0f, -2.7f, 0.7f);
	glVertex3f(-1.0f, -2.7f, -0.7f);
	glVertex3f(-0.1f, -2.7f, -0.7f);
	glVertex3f(-0.1f, -2.7f, 0.7f);

	// Bottom face
	glColor3f(0.0f, 0.0f, 1.0f);  
	glVertex3f(-1.0f, -3.0f, 0.7f);
	glVertex3f(-1.0f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, 0.7f);

	// Front face
	glColor3f(0.0f, 0.0f, 1.0f);  
	glVertex3f(-1.0f, -2.7f, 0.7f);
	glVertex3f(-0.1f, -2.7f, 0.7f);
	glVertex3f(-0.1f, -3.0f, 0.7f);
	glVertex3f(-1.0f, -3.0f, 0.7f);

	// Back face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -2.7f, -0.7f);
	glVertex3f(-0.1f, -2.7f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-1.0f, -3.0f, -0.7f);

	// Left face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -2.7f, 0.7f);
	glVertex3f(-1.0f, -2.7f, -0.7f);
	glVertex3f(-1.0f, -3.0f, -0.7f);
	glVertex3f(-1.0f, -3.0f, 0.7f);

	// Right face
	glColor3f(0.0f, 0.0f, 1.0f);  
	glVertex3f(-0.1f, -2.7f, 0.7f);
	glVertex3f(-0.1f, -2.7f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, 0.7f);
	glEnd(); 
}

static void init(void)
{
	theTorus = glGenLists(1);
	glNewList(theTorus, GL_COMPILE);
	torus(100, 25);
	glEndList();
	glShadeModel(GL_FLAT);
	glClearColor(0.0, 0.0, 0.0, 0.0);
}
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);  
	///////
	if (!projection)
		gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], 0, 1, 0);

	glRotatef(rot[0], 1.0f, 0.0f, 0.0f);
	glRotatef(rot[1], 0.0f, 1.0f, 0.0f);
	glRotatef(rot[2], 0.0f, 0.0f, 1.0f);
	glScalef(scale, scale, scale);
	
	glPushMatrix();
	glColor3fv(color[0]);
	glRotatef(30., .0, 0.0, .0);
	glRotatef(30., -1.0, 0.0, .0);
	glTranslatef(.0f, .0f, 0.2f);
	glTranslatef(-.1f, -1.5f, .0f);
	glScalef(2., 2., 2.);
	torus(100, 25);					//팔!!!!!!!!!!!!!!!!!!!!

	glPopMatrix();
	//////
	glPushMatrix();
	glColor3fv(color[0]);
	glRotatef(30., .0, 0.0, .0);
	glRotatef(30., -1.0, 0.0, .0);
	glTranslatef(.0f, .0f, 0.3f);
	glTranslatef(-.1f, -1.5f, .0f);
	glScalef(-2., 2., 2.);
	torus(100, 25);					//팔!!!!!!!!!!!!!!!!!!!!
	glPopMatrix();
	//////

	//왼쪽 다리
	glPushMatrix();
	glColor3fv(color[0]);
	Leg();
	glPopMatrix();

	//오른쪽 다리
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(1.1f, 0.0f, .0f);
	Leg();
	glPopMatrix();

	//허벅지튀어나온부분 - 실린더
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.f);
	glTranslatef(-0.99f, -1.3f, .4f);
	glRotatef(90., 0.0, 1.0, .0);
	gluQuadricDrawStyle(c, GLU_FILL);
	gluCylinder(c, 0.4, 0.4, 1.98, 20, 20); // 실린더. 스탠드의 body.
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.f);
	glRotatef(90., 1.0, 0.0, .0);
	glTranslatef(0.8f, 0.4f, 1.3f);
	gluQuadricDrawStyle(c, GLU_FILL);
	glutSolidSphere(0.4, 2, 10);
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.f);
	glRotatef(90., 1.0, 0.0, .0);
	glTranslatef(-0.8f, 0.4f, 1.3f);
	gluQuadricDrawStyle(c, GLU_FILL);
	glutSolidSphere(0.4, 2, 10);
	glPopMatrix();

	//왼쪽 발
	glPushMatrix();
	glColor3fv(color[2]);
	foot();
	glPopMatrix();

	//오른쪽 발
	glPushMatrix();
	glColor3fv(color[2]);
	glTranslatef(1.1f, 0.0f, .0f);
	foot();
	glPopMatrix();


	//목 - 실린더
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(.0f, 1.3f, 0.0f);
	glRotatef(90., 1, 0, 0);
	gluQuadricDrawStyle(c, GLU_FILL);
	gluCylinder(c, 0.5, 0.5, .3, 20, 20); // 실린더. 스탠드의 body.
	glPopMatrix();

	//손목 - 실린더
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(1.6f, -0.7f, 0.7f);
	glRotatef(90., 3.2, -1.8, 3.4);
	gluQuadricDrawStyle(c, GLU_FILL);
	gluCylinder(c, 0.25, 0.25, .5, 20, 20); // 실린더. 스탠드의 body.
	glPopMatrix();
	//    - sphere
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(1.6f, -0.7f, 0.7f);
	glRotatef(90., 5.0, -1.0, -1.0);
	glRotatef(90., 3.2, -1.8, 3.4);
	gluQuadricDrawStyle(c, GLU_FILL);
	glutSolidSphere(0.26, 2, 10);
	glPopMatrix();

	//손 - torus
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(1.6f, -1.3f, 1.03f);
	glRotatef(30., 0.0, -0.1, 0.);
	glRotatef(90., 7.0, -6.0, -1.5);
	glRotatef(90., 1.8, -1.5, 1.4);
	glScalef(0.2, 0.2, 0.6);
	torus_hand(100, 25);
	glPopMatrix();

	//손목2 - 실린더
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(-1.8f, -0.7f, 0.7f);
	glRotatef(90., 3.2, -1.8, 3.4);
	gluQuadricDrawStyle(c, GLU_FILL);
	gluCylinder(c, 0.25, 0.25, .5, 20, 20); // 실린더. 스탠드의 body.
	glPopMatrix();
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(-1.8f, -0.7f, 0.7f);
	glRotatef(90., 5.0, -1.0, -1.0);
	glRotatef(90., 3.2, -1.8, 3.4);
	gluQuadricDrawStyle(c, GLU_FILL);
	glutSolidSphere(0.26, 2, 10);
	glPopMatrix();

	//손2 - torus
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(-1.75f, -1.35f, 1.03f);
	glRotatef(10., -1.0, -0.1, 0.);
	glRotatef(90., 7.0, -3.0, -1.5);
	glRotatef(90., 0.8, -1.5, 1.4);
	glScalef(0.2, 0.2, 0.6);
	torus_hand(100, 25);
	glPopMatrix();

	///////////////////////////////////////////////////////
	//얼굴 - 실린더
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);		// 바탕 흰색
	glTranslatef(.0f, 2.3f, 0.0f);
	glRotatef(90., 1, 0, 0);
	gluQuadricDrawStyle(c, GLU_FILL);

	gluQuadricNormals(c, GLU_SMOOTH);
	gluQuadricTexture(c, GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[2]);  // 몸통 front 텍스쳐

	glColor3f(1.0f, 1.0f, 1.0f);
	glRotatef(180., 1, 0, 0);
	glTranslatef(0.0f, 0.f, -1.1f);
	gluCylinder(c, 0.8, 0.8, 1.1, 20, 20); // 실린더. 스탠드의 body.

	glPopMatrix();
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(.0f, 2.3f, 0.0f);
	glRotatef(90., 0, 0, 1);
	gluQuadricDrawStyle(c, GLU_FILL);
	glutSolidSphere(0.8, 2, 10); //얼굴뚜껑
	glPopMatrix();

	glPushMatrix();
	glTranslatef(.0f, -1.1f, 0.0f);
	glTranslatef(.0f, 2.3f, 0.0f);
	glRotatef(90., 0, 0, 1);
	glutSolidSphere(0.8, 2, 10); //얼굴뚜껑
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	//머리뚜껑? 모자?(토러스)
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(0.0f, 2.35f, .03f);
	glRotatef(90., 1.0, 0.0, 0.0);
	glScalef(0.4, 0.4, 0.6);
	torus_hat(100, 25);
	glPopMatrix();


	//몸통
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);  // 몸통 front 텍스쳐

	glBegin(GL_QUADS);
	// Front face  (z = 1.0f)
	glColor3f(1.0f, 1.0f, 1.0f);		// 바탕 흰색
	glTexCoord2f(0.85f, 1.0f);		// 몸통 front 텍스쳐 부분  
	glVertex3f(0.7f, 1.0f, 0.7f);

	glTexCoord2f(0.15f, 1.0f);		// 몸통 front 텍스쳐 부분
	glVertex3f(-0.7f, 1.0f, 0.7f);

	glTexCoord2f(0.0f, 0.0f);			// 몸통 front 텍스쳐 부분
	glVertex3f(-1.0f, -1.0f, 0.7f);

	glTexCoord2f(1.0f, 0.0f);			// 몸통 front 텍스쳐 부분
	glVertex3f(1.0f, -1.0f, 0.7f);
	glEnd();  // end
	glDisable(GL_TEXTURE_2D);


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);  // 몸통 back 텍스쳐
	glBegin(GL_QUADS);
	// Front face  (z = 1.0f)
	glColor3f(1.0f, 1.0f, 1.0f);		// 바탕 흰색
	glTexCoord2f(0.0f, 0.0f);			// 등 front 텍스쳐 부분 
	glVertex3f(1.0f, -1.0f, -0.7f);

	glTexCoord2f(1.0f, 0.0f);			// 등 front 텍스쳐 부분
	glVertex3f(-1.0f, -1.0f, -0.7f);

	glTexCoord2f(0.85f, 1.0f);		// 등 front 텍스쳐 부분
	glVertex3f(-0.7f, 1.0f, -0.7f);

	glTexCoord2f(0.15f, 1.0f);		// 등 front 텍스쳐 부분
	glVertex3f(0.7f, 1.0f, -0.7f);

	glEnd();  // end
	glDisable(GL_TEXTURE_2D);


	glBegin(GL_QUADS);
	// Top face
	glColor3f(0.0f, 0.0f, 1.0f);  
	glVertex3f(0.7f, 1.0f, -0.7f);
	glVertex3f(-0.7f, 1.0f, -0.7f);
	glVertex3f(-0.7f, 1.0f, 0.7f);
	glVertex3f(0.7f, 1.0f, 0.7f);

	// Bottom face
	glColor3f(0.0f, 0.f, 0.8f);  
	glVertex3f(1.0f, -1.0f, 0.7f);
	glVertex3f(-1.0f, -1.0f, 0.7f);
	glVertex3f(-1.0f, -1.0f, -0.7f);
	glVertex3f(1.0f, -1.0f, -0.7f);

	// Left face 
	glColor3f(0.0f, 0.0f, 1.0f); 
	glVertex3f(-0.7f, 1.0f, 0.7f);
	glVertex3f(-0.7f, 1.0f, -0.7f);
	glVertex3f(-1.0f, -1.0f, -0.7f);
	glVertex3f(-1.0f, -1.0f, 0.7f);

	// Right face
	glColor3f(0.0f, 0.0f, 1.0f);   
	glVertex3f(0.7f, 1.0f, -0.7f);
	glVertex3f(0.7f, 1.0f, 0.7f);
	glVertex3f(1.0f, -1.0f, 0.7f);
	glVertex3f(1.0f, -1.0f, -0.7f);
	glEnd();  


	glFlush();
	glutSwapBuffers();
}

void myResize(int width, int height) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (projection)
		glOrtho(-size, size, -size, size, -size, size);
	else
		gluPerspective(60., (float)width / height, .1, 100.);
	glEnable(GL_DEPTH_TEST);
	windowWidth = width;
	windowHeight = height;
}
void myIdle()
{
	anitheta += anithetaDelta;
	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		scale *= scaleDelta;
		break;
	case 'z':
		scale /= scaleDelta;
		break;
	}
	glutPostRedisplay();
}

void myMotion(int x, int y)
{
	if (mouseButton == GLUT_LEFT_BUTTON && mouseState == GLUT_DOWN) {
		rot[1] -= (mouseX - x) / 2.;
		rot[0] -= (mouseY - y) / 2.;
		glutPostRedisplay();
		mouseX = x;
		mouseY = y;
	}
}

void myInit()
{
	c = gluNewQuadric();
	gluQuadricDrawStyle(c, GLU_LINE);

}

void myMouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (aniOn) glutIdleFunc(NULL);
		mouseState = state;
		mouseButton = btn;
		mouseX = x;
		mouseY = y;
	}
	else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		mouseState = -1;
		if (aniOn) glutIdleFunc(myIdle);
	}
	else return;
	glutPostRedisplay();
}

void main(void)
{
	myInit();	init();
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("사람");
	LoadGLTextures();	// 텍스쳐
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myResize);

	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouse);
	glutMotionFunc(myMotion);
	glutIdleFunc(myIdle);
	glutMainLoop();
}

