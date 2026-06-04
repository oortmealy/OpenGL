#include "piplup.h"

// ============================================================
// 컬러 상수 (reference/README.md 컬러 시스템 기준)
// ============================================================
// 머리·등·날개 – 짙은 파랑 #006FCD
static const float BODY_DARK[3]  = {0.00f, 0.44f, 0.80f};
// 몸통 앞면·뺨 – 하늘색 (조명 과다노출 방지를 위해 원색 대비 -20%)
static const float BODY_LIGHT[3] = {0.40f, 0.62f, 0.73f};
// 부리·발 – 노란색 #E7BA54
static const float BEAK_COL[3]   = {0.91f, 0.73f, 0.33f};
// 배 흰점·눈 흰자·얼굴 흰자 #FBFCFB
static const float BELLY[3]      = {0.98f, 0.99f, 0.98f};
// 눈동자 – 검정 #0C0D0C
static const float EYE_COL[3]    = {0.05f, 0.05f, 0.05f};

// ============================================================
// 레퍼런스 기반 좌표 설계
// ============================================================
// 기준: ref_3d_front.png 픽셀 분석 결과
//   전체 키 = 3.3 unit (발바닥 Y≈-1.35, 머리 꼭대기 Y≈2.30)
//   비율 → OpenGL Y좌표 변환: y = (ref_비율 × 3.3) - 1.35
//
//   부위          ref비율  OpenGL Y   반지름/크기
//   ---------------------------------------------------
//   발 바닥        0.00     -1.35
//   발 중심        0.09     -1.05     rx=0.38 ry=0.16 rz=0.28
//   몸통 중심      0.35     -0.20     rx=1.00 ry=0.65 rz=0.82
//   몸통 상단      0.52     +0.37
//   머리 중심      0.82     +1.35     r≈0.95
//   눈 높이        0.72     +1.02
//   부리 높이      0.62     +0.70
//   머리 꼭대기    0.98     +2.28

// ============================================================

// 삼각형 세 꼭짓점으로 면 법선을 계산하고 즉시 설정한다.
// 수동 메시(부리, 날개)처럼 glNormal이 없는 경우에 사용한다.
static void setFaceNormal(const Vec3& a, const Vec3& b, const Vec3& c) {
    Vec3 u = {b.x-a.x, b.y-a.y, b.z-a.z};
    Vec3 v = {c.x-a.x, c.y-a.y, c.z-a.z};
    Vec3 n = {u.y*v.z - u.z*v.y,
              u.z*v.x - u.x*v.z,
              u.x*v.y - u.y*v.x};
    float len = sqrtf(n.x*n.x + n.y*n.y + n.z*n.z);
    if (len > 1e-6f) glNormal3f(n.x/len, n.y/len, n.z/len);
}

void setColor(float r, float g, float b) {
    glColor3f(r, g, b);
}

// ambient/diffuse = col, specular = 흰색 고정, shininess = 부위별 조정
// GL_COLOR_MATERIAL(AMBIENT_AND_DIFFUSE)이 활성화돼 있으므로
// glColor3f로 current color를 설정해야 ambient/diffuse가 제대로 반영된다.
void setMaterial(const float col[3], float shininess) {
    float sp[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glColor3f(col[0], col[1], col[2]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  sp);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// 구를 비율만 다르게 늘려서 타원체처럼 그리는 함수.
// glutSolidSphere(radius=1)에 glScalef(sx,sy,sz)를 적용하면
// 각 축 방향 크기가 sx, sy, sz가 된다.
void drawEllipsoid(float sx, float sy, float sz, float radius) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidSphere(radius, 40, 40);
    glPopMatrix();
}

// y축 방향으로 서 있는 원뿔을 그리는 함수.
// glutSolidCone은 기본적으로 z축 방향이므로 -90° 회전해서 y축으로 세운다.
void drawConeBetweenY(float radius, float height) {
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(radius, height, 24, 12);
    glPopMatrix();
}

// 부리: 삼각뿔 형태의 직접 메시.
// 얼굴 표면에 붙는 삼각형 밑면(z≈0) + 앞으로 튀어나온 꼭짓점(z>0)으로 구성.
void drawBeakMesh() {
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

// 날개: 납작한 삼각뿔 메시.
// 세 외곽점 + 앞쪽으로 살짝 나온 중심점으로 입체감을 준다.
void drawWingMesh() {
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
// 펭도리 전체 모델
// ============================================================
void drawPiplup() {

    // ── 1. 몸통 ────────────────────────────────────────────
    // 몸통 중심 Y=-0.20, rx=1.00(좌우) ry=0.65(높이) rz=0.82(앞뒤)
    glPushMatrix();
    glTranslatef(0.0f, -0.20f, 0.0f);
    setMaterial(BODY_DARK, 60.0f);
    drawEllipsoid(1.00f, 0.65f, 0.82f);
    glPopMatrix();

    // ── 2. 몸통 앞면 (하늘색 배) ───────────────────────────
    // 몸통 앞쪽(z+방향)에 납작한 하늘색 타원체를 붙인다.
    glPushMatrix();
    glTranslatef(0.0f, -0.22f, 0.68f);
    setMaterial(BODY_LIGHT, 30.0f);
    drawEllipsoid(0.70f, 0.50f, 0.10f);
    glPopMatrix();

    // ── 3. 배 흰 점 2개 ────────────────────────────────────
    glPushMatrix();
    glTranslatef(-0.20f, -0.05f, 0.77f);
    setMaterial(BELLY, 30.0f);
    drawEllipsoid(0.14f, 0.14f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.20f, -0.05f, 0.77f);
    setMaterial(BELLY, 30.0f);
    drawEllipsoid(0.14f, 0.14f, 0.05f);
    glPopMatrix();

    // ── 4. 머리 ────────────────────────────────────────────
    // 머리 중심 Y=+1.35, r≈0.95
    glPushMatrix();
    glTranslatef(0.0f, 1.35f, 0.0f);
    setMaterial(BODY_DARK, 60.0f);
    drawEllipsoid(0.95f, 0.95f, 0.87f);
    glPopMatrix();

    // ── 5. 얼굴 흰자 (흰색 얼굴 마스크) ───────────────────
    glPushMatrix();
    glTranslatef(0.0f, 1.18f, 0.75f);
    setMaterial(BELLY, 30.0f);
    drawEllipsoid(0.68f, 0.58f, 0.10f);
    glPopMatrix();

    // ── 6. 왼쪽 눈 ─────────────────────────────────────────
    glPushMatrix();
    glTranslatef(-0.27f, 1.08f, 0.78f);
    setMaterial(EYE_COL, 60.0f);
    drawEllipsoid(0.14f, 0.18f, 0.08f);
    glPopMatrix();

    // ── 7. 오른쪽 눈 ───────────────────────────────────────
    glPushMatrix();
    glTranslatef(0.27f, 1.08f, 0.78f);
    setMaterial(EYE_COL, 60.0f);
    drawEllipsoid(0.14f, 0.18f, 0.08f);
    glPopMatrix();

    // ── 8. 왼쪽 눈 하이라이트 ──────────────────────────────
    glPushMatrix();
    glTranslatef(-0.22f, 1.14f, 0.84f);
    setMaterial(BELLY, 30.0f);
    drawEllipsoid(0.04f, 0.05f, 0.025f);
    glPopMatrix();

    // ── 9. 오른쪽 눈 하이라이트 ────────────────────────────
    glPushMatrix();
    glTranslatef(0.32f, 1.14f, 0.84f);
    setMaterial(BELLY, 30.0f);
    drawEllipsoid(0.04f, 0.05f, 0.025f);
    glPopMatrix();

    // ── 10. 부리 ───────────────────────────────────────────
    // 얼굴 표면(Z≈0.75)에서 앞으로 돌출
    glPushMatrix();
    glTranslatef(0.0f, 0.84f, 0.75f);
    setMaterial(BEAK_COL, 20.0f);
    drawBeakMesh();
    glPopMatrix();

    // ── 11. 왼쪽 날개 ──────────────────────────────────────
    glPushMatrix();
    glTranslatef(-0.90f, 0.00f, 0.08f);
    glRotatef(25.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
    setMaterial(BODY_DARK, 60.0f);
    drawWingMesh();
    glPopMatrix();

    // ── 12. 오른쪽 날개 ────────────────────────────────────
    // glScalef(-1,1,1)로 x축 좌우 반전
    glPushMatrix();
    glTranslatef(0.90f, 0.00f, 0.08f);
    glScalef(-1.0f, 1.0f, 1.0f);
    glRotatef(25.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
    setMaterial(BODY_DARK, 60.0f);
    drawWingMesh();
    glPopMatrix();

    // ── 13. 왼쪽 발 ────────────────────────────────────────
    // 발 중심 Y=-1.05, 몸통 앞쪽에 위치
    glPushMatrix();
    glTranslatef(-0.42f, -1.05f, 0.40f);
    setMaterial(BEAK_COL, 20.0f);
    drawEllipsoid(0.38f, 0.16f, 0.28f);
    glPopMatrix();

    // ── 14. 오른쪽 발 ──────────────────────────────────────
    glPushMatrix();
    glTranslatef(0.42f, -1.05f, 0.40f);
    setMaterial(BEAK_COL, 20.0f);
    drawEllipsoid(0.38f, 0.16f, 0.28f);
    glPopMatrix();
}

// ============================================================
// 바닥 격자
// ============================================================
void drawGround() {
    glDisable(GL_LIGHTING);
    setColor(0.25f, 0.25f, 0.25f);
    glBegin(GL_LINES);
    for (int i = -5; i <= 5; ++i) {
        glVertex3f((float)i, -1.35f, -5.0f);
        glVertex3f((float)i, -1.35f,  5.0f);
        glVertex3f(-5.0f, -1.35f, (float)i);
        glVertex3f( 5.0f, -1.35f, (float)i);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}
