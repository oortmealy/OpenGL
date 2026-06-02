#include <gl/glut.h>
#include <gl/GLAUX.H>
#include <cstdlib>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include "arcball.h"

static float aspect_ratio = 1.0f;
static int width, height;

// scene parameters
const vec eye( 0.0f, 0.0f, -20.0f );
const vec centre( 0.0f, 0.0f, 0.0f );
const vec up( 0.0f, 1.0f, 0.0f );
const float SPHERE_RADIUS = 5.0f;

double zoom_x=.5;

GLuint tex;  

// BMP 파일 로드
AUX_RGBImageRec *LoadBMP(char *Filename){
	FILE *File=NULL;
	if (!Filename) return NULL;
	File=fopen(Filename,"r");
	if(File){
		fclose(File);
		return auxDIBImageLoad(Filename);
	}
	return NULL;                                      
}

// 텍스쳐 로드
void loadTexture(void){  
	AUX_RGBImageRec *texRec;                
	texRec = new AUX_RGBImageRec();
	if(texRec=LoadBMP("image/atom.bmp")){
		printf("load success");
       glGenTextures(1, &tex);   
       glBindTexture(GL_TEXTURE_2D, tex);                                           
          
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);         
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);         

       glTexImage2D(GL_TEXTURE_2D, 0, 3, texRec->sizeX, texRec->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texRec->data);      
    }else
		printf("load failed");
    if(texRec){
		if(texRec->data) free(texRec->data); 
		free(texRec);
    }
    glEnable(GL_TEXTURE_2D);
}  

// 뷰 초기화
static void reset_view(int w, int h){
    width = w;
    height = h;
    aspect_ratio = (float) width / (float) height;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 50.0f, aspect_ratio, 1.0f, 50.0f );
    gluLookAt(
              eye.x, eye.y, eye.z,
              centre.x, centre.y, centre.z,
              up.x, up.y, up.z );
    // arcball 초기화
    arcball_setzoom( SPHERE_RADIUS, eye, up );
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}


/* <START> arcball 사용을 위한 보조 함수들*/
inline float randf()
{
    return ((1.0f / 127.f) * (((float)(rand() & 255)) - 128.0f)) ;
}

inline vec rotate_x( vec v, float sin_ang, float cos_ang )
{
    return vec(
               v.x,
               (v.y * cos_ang) + (v.z * sin_ang),
               (v.z * cos_ang) - (v.y * sin_ang)
               );
}

inline vec rotate_y( vec v, float sin_ang, float cos_ang )
{
    return vec(
               (v.x * cos_ang) + (v.z * sin_ang),
               v.y,
               (v.z * cos_ang) - (v.x * sin_ang)
               );
}

static void click_scene(int x, int y)
{
    int invert_y = (height - y) - 1; // OpenGL viewport coordinates are Cartesian
    arcball_start(x,invert_y);
}

static void drag_scene(int x, int y)
{
    int invert_y = (height - y) - 1;
    arcball_move(x,invert_y);
}
/* <END> arcball 사용을 위한 보조 함수들 */


// sphere를 그리는 함수
void renderSphere( float cx, float cy, float cz, float r, int p ) {  
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
  
    if(r < 0)  
        r = -r;  
  
    if(p < 0)  
        p = -p;  
  
    if(p < 4 || r <= 0) {  
        glBegin( GL_POINTS );  
        glVertex3f( cx, cy, cz );  
        glEnd();  
        return;  
    }  
  
    for(i = 0; i < p/2; ++i ) {  
        theta1 = i * TWOPI / p - PIDIV2;  
        theta2 = (i + 1) * TWOPI / p - PIDIV2;  
  
        glBegin(GL_TRIANGLE_STRIP);   
        {  
            for(j = 0; j <= p; ++j )  
            {  
                theta3 = j * TWOPI / p;  
  
                ex = cos(theta2) * cos(theta3);  
                ey = sin(theta2);  
                ez = cos(theta2) * sin(theta3);  
                px = cx + r * ex;  
                py = cy + r * ey;  
                pz = cz + r * ez;  
  
                glNormal3f( ex, ey, ez );  
                glTexCoord2f( -(j/(float)p) , 2*(i+1)/(float)p );  
                glVertex3f( px, py, pz );  
  
                ex = cos(theta1) * cos(theta3);  
                ey = sin(theta1);  
                ez = cos(theta1) * sin(theta3);  
                px = cx + r * ex;  
                py = cy + r * ey;  
                pz = cz + r * ez;  
  
                glNormal3f( ex, ey, ez );  
                glTexCoord2f( -(j/(float)p), 2*i/(float)p );  
                glVertex3f( px, py, pz );  
            }  
        }  
        glEnd();  
    }  
}

//  머리 그리는 함수(Texture mapping 사용)
static void draw_head(){
	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
    GLfloat diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    GLfloat spec[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0,GL_SPECULAR,ambient);
 
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
 
    glPushMatrix();
	GLUquadric *quad;

    quad = gluNewQuadric();

	glTranslatef(0, 4, 0);
	glBindTexture(GL_TEXTURE_2D, tex);           //텍스쳐를 입힘   

    renderSphere(0,0,0,4,50);
	glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}


// 작은뿔
static void draw_hair2(){
    glPushMatrix();
    
    glColor3f(0.157f, 0.137f, 0.169f);
    glTranslatef(-2.4f, 5.0f, .8f);
    glRotatef(45, 0, 0, 1);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(2.0f, 4.0f, 32, 32);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// 큰뿔
static void draw_hair1(){
    glPushMatrix();
	
    glColor3f(0.157f, 0.137f, 0.169f);
    glTranslatef(1.2f, 5.3f, .3f);
    glRotatef(-90.0f, 1, 0, 0);
    glRotatef(10.0f, 0, 1, 1);
    glutSolidCone(2.7f, 5.5f, 32, 32);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}


// 코
static void draw_nose(){
    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glPushMatrix();
	
    glColor3f(0.157f, 0.137f, 0.169f);
    glTranslatef(0, 1.2f, -2.9f);
    glScalef(2, 1.5f, 1.5f);
    gluSphere(quad, 0.15f, 36, 36);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}


// 귀
static void draw_ear2(){
    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glPushMatrix();
    glColor3f(0.965f, 0.741f, 0.584f);
    glTranslatef(-3.5f, 2.7f, .0f);
    glScalef(1, 1, 0.25f);
    gluSphere(quad, 1.0f, 36, 36);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// 다른 귀
static void draw_ear1(){
    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glPushMatrix();
    glColor3f(0.965f, 0.741f, 0.584f);
    glTranslatef(3.5f, 2.7f, .0f);
    glScalef(1, 1, 0.25f);
    gluSphere(quad, 1.0f, 36, 36);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// 몸통 윗부분
static void draw_body_top(){
    glPushMatrix();
    
    glColor3f(0.965f, 0.741f, 0.584f);

    glTranslatef(0.0f, -2.0f, 0.0f);
    glRotated(-90, 1.0f, 0.0f, 0.0f);
    glutSolidCone(2.4f, 6.0f, 40, 2);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// 몸통 중간부분
static void draw_body_middle(){
    glPushMatrix();
    
    glColor3f(0.161f, 0.416f, 0.208f);

    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glTranslatef(0.0f, -2.0f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quad, 2.4f, 2.4f, 1.5f, 32, 32);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}


//몸통 아랫부분
static void draw_body_bottom(){
    glPushMatrix();
    
    glColor3f(0.157f, 0.137f, 0.169f);

    GLUquadric *quad;
    quad = gluNewQuadric();

    glTranslatef(0.0f, -3.5f, 0.0f);
    gluSphere(quad, 2.4f, 40.0f, 40.0f);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}


// 오른쪽다리
static void draw_leg_right(){
    glPushMatrix();
    
    glColor3f(0.843f, 0.137f, 0.153f);

    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glTranslatef(-1.15f, -8.0f, 0.0f);
    glRotated(-90, 1.0f, 0.0f, 0.0f);
    glutSolidCone(1.2f, 6.0f, 40, 2);
    glScalef(1.0f, 1.0f, 0.5f);
    
    glColor3f(0.843f, 0.137f, 0.153f);
    gluSphere(quad, 1.2f, 10.0f, 10.0f);
    glPopMatrix();
    
    glPushMatrix();
    
    glColor3f(0.965f, 0.741f, 0.584f);
    glTranslatef(-1.15f, -6.25f, 0.0f);
    glRotated(-90, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.85f, 5.0f, 40, 2);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// 왼쪽다리
static void draw_leg_left(){
    glPushMatrix();

    glColor3f(0.843f, 0.137f, 0.153f);

    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glTranslatef(1.15f, -8.0f, 0.0f);
    glRotated(-90, 1.0f, 0.0f, 0.0f);
    glutSolidCone(1.2f, 6.0f, 40, 2);
    glScalef(1.0f, 1.0f, 0.5f);
    
    glColor3f(0.843f, 0.137f, 0.153f);
    gluSphere(quad, 1.2f, 10.0f, 10.0f);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
    
    glPushMatrix();
    
    glColor3f(0.965f, 0.741f, 0.584f);
    glTranslatef(1.15f, -6.25f, 0.0f);
    glRotated(-90, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.85f, 5.0f, 40, 2);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// 오른쪽 신발
static void draw_shoe_right(){
    glPushMatrix();
    
    glColor3f(0.843f, 0.137f, 0.153f);
    
    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glTranslatef(-1.15f, -6.5f, 0.0f);
    glScalef(1.05f, 0.35f, 1.0f);
    gluSphere(quad, 1.15f, 40.0f, 40.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// 왼쪽 신발
static void draw_shoe_left(){
    glPushMatrix();
    
    glColor3f(0.843f, 0.137f, 0.153f);
    
    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glTranslatef(1.15f, -6.5f, 0.0f);
    glScalef(1.05f, 0.35f, 1.0f);
    gluSphere(quad, 1.15f, 40.0f, 40.0f);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// 왼쪽 팔
static void draw_arm_left(){
    glPushMatrix();
    glColor3f(0.965f, 0.741f, 0.584f);

    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glTranslatef(1.0f, -0.4f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(65.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quad, 0.75f, 0.75f, 3.8f, 32, 32);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}


// 오른쪽 팔
static void draw_arm_right(){
    glPushMatrix();
    glColor3f(0.965f, 0.741f, 0.584f);

    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glTranslatef(-1.0f, -0.4f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-65.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quad, 0.75f, 0.75f, 3.8f, 32, 32);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// 왼쪽 손
static void draw_hand_left(){
    glPushMatrix();
    glColor3f(0.965f, 0.741f, 0.584f);

    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glTranslatef(4.0f, -1.9f, 0.0f);
    gluSphere(quad, 1.2f, 40.0f, 40.0f);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// 오른쪽 손
static void draw_hand_right(){
    glPushMatrix();
    glColor3f(0.965f, 0.741f, 0.584f);

    GLUquadric *quad;
    quad = gluNewQuadric();
    
    glTranslatef(-4.0f, -1.9f, 0.0f);
    gluSphere(quad, 1.25f, 40.0f, 40.0f);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}


// 화면 그리기
static void draw_scene(){
    glEnable(GL_LIGHTING);	// 조명 활성화    
    glPushMatrix();
    glDisable( GL_DEPTH_TEST );
    glTranslatef( eye.x, eye.y, eye.z );
    arcball_rotate();
    glEnable( GL_DEPTH_TEST );
    glPopMatrix();
    
    arcball_rotate();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    
    /* <START> draw 함수들 호출 */
    draw_head();
    
	draw_nose();
    draw_ear2();
    draw_ear1();
    draw_hair1();
    draw_hair2();
    
    draw_body_top();
    draw_body_bottom();
    draw_body_middle();

    draw_shoe_left();
    draw_shoe_right();

    draw_leg_left();
    draw_leg_right();
    draw_arm_left();
    draw_arm_right();
    draw_hand_left();
    draw_hand_right();
	/* <END> draw 함수들 호출 */
}

/* <START> callback 함수들 선언 */
static void resize(int w, int h)
{
    reset_view(w,h);
}

static void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glScalef(zoom_x, zoom_x, 1.0f );
    
    draw_scene();
    
    glutSwapBuffers();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;
        case 'a' :
            zoom_x += .05;
            break;
        case 'z' :
            zoom_x -= .05;
            break;
        default :
            break;
    }
    glutPostRedisplay();
}

static void idle()
{
    glutPostRedisplay();
}

static void mouse_button(int button, int state, int x, int y)
{
    if ( state == GLUT_DOWN ) click_scene(x,y);
}

static void mouse_motion(int x, int y)
{
    // glutMotionFunc only called when a mouse button is held down
    drag_scene(x,y);
}

/* <END> callback 함수들 선언 */

//  조명 설정 함수
void TurnOnLight(void) {
    GLfloat mat_specular[] = { 100.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { 127.0 };
    GLfloat light_position[] = { 1.0f, 1.0f, -7.0f, 0.0f };
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glEnable(GL_LIGHT0);
}

//  초기화 함수
void init(void) {
    glClearColor(.8, .9, 1, 0.0);   // 배경색 설정
    glEnable(GL_DEPTH_TEST);
	loadTexture();
    TurnOnLight();// 조명설정
}


//  main 함수
int main(int argc, char ** argv)
{
    arcball_reset();
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600,800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Astro Boy");
    init();
    
    //콜백 함수 등록
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    glutMouseFunc(mouse_button);
    glutMotionFunc(mouse_motion);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    
    glutMainLoop();
    
    return 0;
}