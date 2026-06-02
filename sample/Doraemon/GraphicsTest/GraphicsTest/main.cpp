#include <glut.h>
#include "SceneNode.h"
#include "HumanParts.h"
#include "texture_loader.h"

SceneNode* rootNode = nullptr;
float zoomZ = -5.0f;
float rotationX = 0.0f, rotationY = 0.0f;
int prevMouseX = -1, prevMouseY = -1;

// 디스플레이 콜백
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, zoomZ);
    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);

    if (rootNode)
        rootNode->render();

    glutSwapBuffers();
}

// 창 리사이즈 콜백
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / (float)h, 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

// 키보드 입력 콜백
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC
    if (key == 'a') zoomZ += 0.5f;
    if (key == 'z') zoomZ -= 0.5f;
    glutPostRedisplay();
}

// 마우스 드래그 회전
void mouseMotion(int x, int y) {
    if (prevMouseX >= 0 && prevMouseY >= 0) {
        rotationY += (x - prevMouseX);
        rotationX += (y - prevMouseY);
    }
    prevMouseX = x;
    prevMouseY = y;
    glutPostRedisplay();
}

// 마우스 버튼 클릭 시 좌표 초기화
void mouseButton(int button, int state, int x, int y) {
    if (state == GLUT_UP) {
        prevMouseX = -1;
        prevMouseY = -1;
    }
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);  // 스케일링된 메쉬 조명 보정
    glEnable(GL_TEXTURE_2D);  // 텍스처 활성화

    loadFaceTexture("face_texture.png"); 

    GLfloat lightPos[] = { 0.0f, 5.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Virtual Human (Scene Graph)");

    init();

    rootNode = buildHuman();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseButton);

    glutMainLoop();
    return 0;
}