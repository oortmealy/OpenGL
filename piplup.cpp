#include "piplup.h"
#include "body.h"
#include "face.h"

// ============================================================
// 컬러 상수 (reference/README.md 컬러 시스템 기준)
// body.cpp / face.cpp에서도 쓰이므로 piplup.h에 extern으로 공개한다.
// ============================================================
// 망토 – 짙은 파랑 #006FCD
const float BODY_DARK[3]  = {0.00f, 0.44f, 0.80f};
// 몸통 앞면·뺨 – 연한 하늘색 #88C8E3
const float BODY_LIGHT[3] = {0.533f, 0.784f, 0.890f};
// 부리·발 – 노란색 #E7BA54
const float BEAK_COL[3]   = {0.91f, 0.73f, 0.33f};
// 배 흰점·눈 흰자·얼굴 흰자 #FBFCFB
const float BELLY[3]      = {0.98f, 0.99f, 0.98f};
// 눈동자 – 검정 #0C0D0C
const float EYE_COL[3]    = {0.05f, 0.05f, 0.05f};

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
void setFaceNormal(const Vec3& a, const Vec3& b, const Vec3& c) {
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

// ============================================================
// 펭도리 전체 모델
// 몸통은 body.cpp, 얼굴은 face.cpp에 분리되어 있다.
// ============================================================
void drawPiplup() {
    drawBody();
    drawFace();
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
    if (enableLighting) {
        glEnable(GL_LIGHTING);
    }
}
