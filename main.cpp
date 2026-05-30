#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cstdlib>

// 화면에 그릴 내용
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);  // 빨간색
        glVertex2f( 0.0f,  0.5f);
        glColor3f(0.0f, 1.0f, 0.0f);  // 초록색
        glVertex2f(-0.5f, -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f);  // 파란색
        glVertex2f( 0.5f, -0.5f);
    glEnd();

    glFlush();
}

// 키보드 입력 처리
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {  // ESC 키
        exit(0);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL - Hello Triangle");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // 배경색 (어두운 회색)

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
