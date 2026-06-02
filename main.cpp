#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdlib>

float cameraDistance = 7.0f;
float rotateX = 15.0f;
float rotateY = -25.0f;
bool isDragging = false;
int lastMouseX = 0;
int lastMouseY = 0;

struct Vec3 {
    float x;
    float y;
    float z;
};

void setColor(float r, float g, float b) {
    glColor3f(r, g, b);
}

void drawEllipsoid(float sx, float sy, float sz, float radius = 1.0f) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidSphere(radius, 40, 40);
    glPopMatrix();
}

void drawConeBetweenY(float radius, float height) {
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(radius, height, 24, 12);
    glPopMatrix();
}

void drawBeakMesh() {
    Vec3 vertices[] = {
        { 0.0f,  0.20f,  0.55f},
        {-0.25f, -0.12f,  0.55f},
        { 0.25f, -0.12f,  0.55f},
        { 0.0f,  0.02f,  1.05f}
    };

    int faces[][3] = {
        {0, 1, 3},
        {1, 2, 3},
        {2, 0, 3},
        {0, 2, 1}
    };

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 4; ++i) {
        Vec3 a = vertices[faces[i][0]];
        Vec3 b = vertices[faces[i][1]];
        Vec3 c = vertices[faces[i][2]];
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
    }
    glEnd();
}

void drawWingMesh() {
    Vec3 vertices[] = {
        { 0.00f,  0.55f, 0.0f},
        {-0.32f, -0.40f, 0.0f},
        { 0.22f, -0.58f, 0.0f},
        { 0.00f,  0.10f, 0.22f}
    };

    int faces[][3] = {
        {0, 1, 3},
        {1, 2, 3},
        {2, 0, 3},
        {0, 2, 1}
    };

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 4; ++i) {
        Vec3 a = vertices[faces[i][0]];
        Vec3 b = vertices[faces[i][1]];
        Vec3 c = vertices[faces[i][2]];
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
    }
    glEnd();
}

void drawPiplup() {
    setColor(0.12f, 0.45f, 0.95f);
    drawEllipsoid(1.05f, 1.25f, 0.82f);

    glPushMatrix();
    glTranslatef(0.0f, -0.12f, 0.62f);
    setColor(0.88f, 0.96f, 1.0f);
    drawEllipsoid(0.62f, 0.82f, 0.18f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 1.28f, 0.02f);
    setColor(0.04f, 0.17f, 0.55f);
    drawEllipsoid(0.93f, 0.93f, 0.83f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.28f, 1.40f, 0.72f);
    setColor(0.02f, 0.02f, 0.025f);
    drawEllipsoid(0.13f, 0.18f, 0.08f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.28f, 1.40f, 0.72f);
    setColor(0.02f, 0.02f, 0.025f);
    drawEllipsoid(0.13f, 0.18f, 0.08f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.23f, 1.47f, 0.79f);
    setColor(1.0f, 1.0f, 1.0f);
    drawEllipsoid(0.04f, 0.05f, 0.025f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.33f, 1.47f, 0.79f);
    setColor(1.0f, 1.0f, 1.0f);
    drawEllipsoid(0.04f, 0.05f, 0.025f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 1.13f, 0.50f);
    setColor(1.0f, 0.78f, 0.18f);
    drawBeakMesh();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.93f, 0.18f, 0.08f);
    glRotatef(28.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-18.0f, 0.0f, 1.0f, 0.0f);
    setColor(0.04f, 0.17f, 0.55f);
    drawWingMesh();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.93f, 0.18f, 0.08f);
    glScalef(-1.0f, 1.0f, 1.0f);
    glRotatef(28.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-18.0f, 0.0f, 1.0f, 0.0f);
    setColor(0.04f, 0.17f, 0.55f);
    drawWingMesh();
    glPopMatrix();

    for (int i = -1; i <= 1; ++i) {
        glPushMatrix();
        glTranslatef(i * 0.22f, 2.18f, 0.02f);
        glRotatef(i * 20.0f, 0.0f, 0.0f, 1.0f);
        setColor(0.18f, 0.62f, 1.0f);
        drawConeBetweenY(0.11f, 0.45f);
        glPopMatrix();
    }

    glPushMatrix();
    glTranslatef(-0.45f, -1.18f, 0.42f);
    setColor(1.0f, 0.78f, 0.18f);
    drawEllipsoid(0.38f, 0.16f, 0.28f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.45f, -1.18f, 0.42f);
    setColor(1.0f, 0.78f, 0.18f);
    drawEllipsoid(0.38f, 0.16f, 0.28f);
    glPopMatrix();
}

void drawGround() {
    glDisable(GL_LIGHTING);
    setColor(0.25f, 0.25f, 0.25f);
    glBegin(GL_LINES);
    for (int i = -5; i <= 5; ++i) {
        glVertex3f((float)i, -1.35f, -5.0f);
        glVertex3f((float)i, -1.35f, 5.0f);
        glVertex3f(-5.0f, -1.35f, (float)i);
        glVertex3f(5.0f, -1.35f, (float)i);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 1.1f, cameraDistance, 0.0f, 0.35f, 0.0f, 0.0f, 1.0f, 0.0f);

    GLfloat lightPosition[] = {3.0f, 5.0f, 5.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    drawGround();

    glPushMatrix();
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
    drawPiplup();
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    if (height == 0) {
        height = 1;
    }

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int) {
    if (key == 27) {
        exit(0);
    }

    if (key == 'a' || key == 'A') {
        cameraDistance = fmaxf(3.0f, cameraDistance - 0.3f);
    } else if (key == 'z' || key == 'Z') {
        cameraDistance = fminf(12.0f, cameraDistance + 0.3f);
    }

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        isDragging = (state == GLUT_DOWN);
        lastMouseX = x;
        lastMouseY = y;
    }
}

void motion(int x, int y) {
    if (!isDragging) {
        return;
    }

    rotateY += (x - lastMouseX) * 0.5f;
    rotateX += (y - lastMouseY) * 0.5f;
    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

void init() {
    glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat ambient[] = {0.28f, 0.28f, 0.28f, 1.0f};
    GLfloat diffuse[] = {0.85f, 0.85f, 0.85f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 700);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL - Simple 3D Piplup");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}
