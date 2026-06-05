#include "piplup.h"
#include "face.h"

// 부리: 삼각뿔 형태의 직접 메시.
// 얼굴 표면에 붙는 삼각형 밑면(z≈0) + 앞으로 튀어나온 꼭짓점(z>0)으로 구성.
static void drawBeakMesh() {
    Vec3 vertices[] = {
        { 0.00f,  0.20f, 0.0f},  // 0: 밑면 위쪽
        {-0.22f, -0.10f, 0.0f},  // 1: 밑면 왼쪽 아래
        { 0.22f, -0.10f, 0.0f},  // 2: 밑면 오른쪽 아래
        { 0.00f,  0.04f, 0.48f}  // 3: 앞으로 돌출된 부리 끝
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
// 얼굴 파트 개별 그리기 함수
// ============================================================

// 머리: 짙은 파랑 타원체. 머리 중심 Y=+1.35, r≈0.95
static void drawHead() {
    glPushMatrix();
    glTranslatef(0.0f, 1.35f, 0.0f);
    setMaterial(BODY_DARK, 60.0f);
    drawEllipsoid(0.95f, 0.95f, 0.87f);
    glPopMatrix();
}

// 얼굴 흰자(흰색 얼굴 마스크).
static void drawFaceMask() {
    glPushMatrix();
    glTranslatef(0.0f, 1.18f, 0.75f);
    setMaterial(BELLY, 30.0f);
    drawEllipsoid(0.68f, 0.58f, 0.10f);
    glPopMatrix();
}

// 눈 1개: 눈동자 + 흰 하이라이트를 함께 그린다.
// 좌우 대칭이라 x좌표만 받아서 공용으로 그린다.
static void drawEye(float x) {
    // 눈동자
    glPushMatrix();
    glTranslatef(x, 1.08f, 0.78f);
    setMaterial(EYE_COL, 60.0f);
    drawEllipsoid(0.14f, 0.18f, 0.08f);
    glPopMatrix();

    // 눈 하이라이트
    glPushMatrix();
    glTranslatef(x, 1.11f, 0.84f);
    setMaterial(BELLY, 30.0f);
    drawEllipsoid(0.04f, 0.051f, 0.025f);
    glPopMatrix();
}

// 양쪽 눈.
static void drawEyes() {
    drawEye(-0.27f);
    drawEye(0.27f);
}

// 부리: 얼굴 표면(Z≈0.75)에서 앞으로 돌출.
static void drawBeak() {
    glPushMatrix();
    glTranslatef(0.0f, 0.84f, 0.75f);
    setMaterial(BEAK_COL, 20.0f);
    drawBeakMesh();
    glPopMatrix();
}

// ============================================================
// 얼굴 전체: 머리·얼굴 마스크·눈·부리.
// ============================================================
void drawFace() {
    drawHead();
    drawFaceMask();
    drawEyes();
    drawBeak();
}
