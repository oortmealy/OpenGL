#include "piplup.h"
#include "body.h"

// 날개: 납작한 삼각뿔 메시.
// 세 외곽점 + 앞쪽으로 살짝 나온 중심점으로 입체감을 준다.
static void drawWingMesh() {
    Vec3 vertices[] = {
        { 0.00f,  0.50f, 0.00f},
        {-0.30f, -0.38f, 0.00f},
        { 0.20f, -0.52f, 0.00f},
        { 0.00f,  0.08f, 0.20f}
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
        setFaceNormal(a, b, c);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
    }
    glEnd();
}

// ============================================================
// 몸통 파트 개별 그리기 함수
// ============================================================

// 몸통 본체: 짙은 파랑 타원체.
// 몸통 중심 Y=-0.20, rx=1.00(좌우) ry=0.65(높이) rz=0.82(앞뒤)
static void drawTorso() {
    glPushMatrix();
    glTranslatef(0.0f, -0.20f, 0.0f);
    setMaterial(BODY_DARK, 60.0f);
    drawEllipsoid(1.00f, 0.65f, 0.82f);
    glPopMatrix();
}

// 몸통 앞면(하늘색 배): 몸통 앞쪽(z+방향)에 납작한 하늘색 타원체를 붙인다.
static void drawBellyFront() {
    glPushMatrix();
    glTranslatef(0.0f, -0.22f, 0.68f);
    setMaterial(BODY_LIGHT, 30.0f);
    drawEllipsoid(0.70f, 0.50f, 0.10f);
    glPopMatrix();
}

// 배 흰 점 1개: 좌우 대칭이라 x좌표만 받아서 공용으로 그린다.
static void drawBellySpot(float x) {
    glPushMatrix();
    glTranslatef(x, -0.05f, 0.77f);
    setMaterial(BELLY, 30.0f);
    drawEllipsoid(0.14f, 0.14f, 0.05f);
    glPopMatrix();
}

// 배 흰 점 2개.
static void drawBellySpots() {
    drawBellySpot(-0.20f);
    drawBellySpot(0.20f);
}

// 날개 1개: x좌표와 좌우 반전 여부를 받아서 공용으로 그린다.
// 오른쪽 날개는 glScalef(-1,1,1)로 x축 좌우 반전한다.
static void drawWing(float x, bool mirror) {
    glPushMatrix();
    glTranslatef(x, 0.00f, 0.08f);
    if (mirror) glScalef(-1.0f, 1.0f, 1.0f);
    glRotatef(25.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
    setMaterial(BODY_DARK, 60.0f);
    drawWingMesh();
    glPopMatrix();
}

// 양쪽 날개.
static void drawWings() {
    drawWing(-0.90f, false);  // 왼쪽
    drawWing(0.90f, true);    // 오른쪽 (좌우 반전)
}

// 발 1개: 좌우 대칭이라 x좌표만 받아서 공용으로 그린다.
// 발 중심 Y=-1.05, 몸통 앞쪽에 위치
static void drawFoot(float x) {
    glPushMatrix();
    glTranslatef(x, -1.05f, 0.40f);
    setMaterial(BEAK_COL, 20.0f);
    drawEllipsoid(0.38f, 0.16f, 0.28f);
    glPopMatrix();
}

// 양쪽 발.
static void drawFeet() {
    drawFoot(-0.42f);
    drawFoot(0.42f);
}

// ============================================================
// 몸통 전체: 몸통 본체·배·날개·발.
// ============================================================
void drawBody() {
    drawTorso();
    drawBellyFront();
    drawBellySpots();
    drawWings();
    drawFeet();
}
