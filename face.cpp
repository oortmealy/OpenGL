#include "piplup.h"
#include "face.h"
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// 머리 텍스처 ID (0 = 미로드)
static GLuint g_headTex = 0;
static GLuint g_headBackTex = 0;

static void loadTexture(const char* path, GLuint& texId) {
    int w, h, ch;
    stbi_set_flip_vertically_on_load(1);  // OpenGL UV는 Y가 아래→위
    unsigned char* data = stbi_load(path, &w, &h, &ch, 4);
    if (!data) return;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // S(가로): 좌우 반전 기준을 중심으로 맞추기 위해 MIRRORED_REPEAT 사용
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    stbi_image_free(data);
}

void initHeadTexture(const char* frontPath, const char* backPath) {
    loadTexture(frontPath, g_headTex);
    loadTexture(backPath, g_headBackTex);
}

// ── 머리 파라미터 ────────────────────────────────────────────
//   HEAD_RZ 하나만 바꾸면 눈·부리 Z가 자동으로 머리 표면에 붙음
static const float HEAD_CY = 1.35f;
static const float HEAD_RX = 1.10f;
static const float HEAD_RY = 0.95f;
static const float HEAD_RZ = 0.90f;  // ← 앞뒤 두께 조절

// 머리 타원체 앞면 Z 계산: (x, y) 위치의 표면 Z를 반환
static float headSurfaceZ(float x, float y) {
    float tx = x / HEAD_RX;
    float ty = (y - HEAD_CY) / HEAD_RY;
    float inner = 1.0f - tx*tx - ty*ty;
    if (inner < 0.0f) inner = 0.0f;
    return HEAD_RZ * sqrtf(inner);
}

static void useBaseLighting() {
    if (!enableLighting) return;

    GLfloat ambient[] = {0.36f, 0.36f, 0.36f, 1.0f};
    GLfloat diffuse[] = {0.72f, 0.72f, 0.72f, 1.0f};
    GLfloat specular[] = {0.24f, 0.24f, 0.22f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}

static void useStrongFaceLighting() {
    if (!enableLighting) return;

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat ambient[] = {0.30f, 0.30f, 0.30f, 1.0f};
    GLfloat diffuse[] = {1.00f, 1.00f, 1.00f, 1.0f};
    GLfloat specular[] = {1.00f, 1.00f, 0.92f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}

// ── 부리 사이즈 ─────────────────────────────────────────────
//   rx(가로 반폭) : 0.220  (= ref ~76, 눈 가로와 유사)
//   ry(세로 반폭) : 0.220  (= ref ~76, 전체 높이 0.440)
//   rz(돌출 깊이) : 0.200  (클수록 볼록, 작을수록 납작)
// ────────────────────────────────────────────────────────────

// ============================================================
// 얼굴 파트 개별 그리기 함수
// ============================================================

// 머리: 짙은 파랑 타원체.
static void drawHead() {
    glPushMatrix();
    glTranslatef(0.0f, HEAD_CY, 0.0f);

    // Pass 1: 전체 타원체를 기본색으로 → 뒤통수는 여기서 처리
    setMaterial(BODY_DARK, 60.0f);
    drawEllipsoid(HEAD_RX, HEAD_RY, HEAD_RZ);

    if (g_headTex) {
        // Pass 2: 앞쪽 반구(로컬 z ≥ 0)만 텍스처로 덧씌움
        // 클립 평면: 로컬 좌표계 기준 z ≥ 0
        GLdouble clipPlane[] = {0.0, 0.0, 1.0, 0.0};
        glClipPlane(GL_CLIP_PLANE0, clipPlane);
        glEnable(GL_CLIP_PLANE0);

        // Pass 1과 깊이값이 같으므로 LEQUAL + 폴리곤 오프셋으로 z-fighting 방지
        glDepthFunc(GL_LEQUAL);
        glPolygonOffset(-1.0f, -1.0f);
        glEnable(GL_POLYGON_OFFSET_FILL);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_headTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // UV 자동 생성 (GL_OBJECT_LINEAR)
        // S: U=0(x=0 중심), U=±1(x=±1 귀) → GL_MIRRORED_REPEAT으로 중심 기준 좌우 대칭
        // T: V=0(y=-1 아래), V=1(y=+1 위)
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        GLfloat sPlane[] = {1.0f, 0.0f, 0.0f, 0.0f};
        GLfloat tPlane[] = {0.0f, 0.5f, 0.0f, 0.5f};
        glTexGenfv(GL_S, GL_OBJECT_PLANE, sPlane);
        glTexGenfv(GL_T, GL_OBJECT_PLANE, tPlane);

        static const float WHITE[] = {1.0f, 1.0f, 1.0f};
        setMaterial(WHITE, 60.0f);
        drawEllipsoid(HEAD_RX, HEAD_RY, HEAD_RZ);

        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDepthFunc(GL_LESS);
        glDisable(GL_CLIP_PLANE0);
    }

    if (g_headBackTex) {
        // Pass 3: 뒤쪽 반구(로컬 z ≤ 0)를 head_back.png 텍스처로 씌움
        GLdouble clipBack[] = {0.0, 0.0, -1.0, 0.0};  // z ≤ 0 영역만
        glClipPlane(GL_CLIP_PLANE0, clipBack);
        glEnable(GL_CLIP_PLANE0);

        glDepthFunc(GL_LEQUAL);
        glPolygonOffset(-1.0f, -1.0f);
        glEnable(GL_POLYGON_OFFSET_FILL);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_headBackTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // UV 자동 생성
        // S: 뒤에서 봤을 때 좌우 대칭이 되도록 X 축을 반전(-1)
        // T: 앞과 동일 (Y*0.5 + 0.5)
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        GLfloat sPlaneBack[] = {-1.0f, 0.0f, 0.0f, 0.0f};
        GLfloat tPlaneBack[] = {0.0f, 0.5f, 0.0f, 0.5f};
        glTexGenfv(GL_S, GL_OBJECT_PLANE, sPlaneBack);
        glTexGenfv(GL_T, GL_OBJECT_PLANE, tPlaneBack);

        static const float WHITE[] = {1.0f, 1.0f, 1.0f};
        setMaterial(WHITE, 60.0f);
        drawEllipsoid(HEAD_RX, HEAD_RY, HEAD_RZ);

        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDepthFunc(GL_LESS);
        glDisable(GL_CLIP_PLANE0);
    }

    if (enableLighting) {
        glEnable(GL_LIGHTING);
    }
    glPopMatrix();
}

// 눈 1개: 눈동자 + 흰 하이라이트를 함께 그린다.
// 좌우 대칭이라 x좌표만 받아서 공용으로 그린다.
static void drawEye(float x) {
    static const float EYE_Y  = 1.39f;
    static const float EYE_RZ = 0.08f;
    float ez = headSurfaceZ(x, EYE_Y);

    // 눈동자
    glPushMatrix();
    glTranslatef(x, EYE_Y, ez);
    setMaterial(EYE_COL, 120.0f);
    drawEllipsoid(0.174f, 0.290f, EYE_RZ);
    glPopMatrix();

    // 눈 하이라이트 (눈 앞면에 평면으로 부착)
    glPushMatrix();
    glTranslatef(x * 0.92f, 1.48f, ez + EYE_RZ + 0.002f);
    setMaterial(BELLY, 90.0f);
    drawEllipsoid(0.075f, 0.120f, 0.002f);
    glPopMatrix();
}

// 양쪽 눈.
static void drawEyes() {
    drawEye(-0.50f);
    drawEye(0.50f);
}

// 부리: 머리 표면에 자동 부착.
static void drawBeak() {
    static const float BEAK_Y  = 1.25f;
    static const float BEAK_RX = 0.220f;
    static const float BEAK_RZ = 0.200f;  // 돌출 깊이
    float bz = headSurfaceZ(0.0f, BEAK_Y);

    glPushMatrix();
    glTranslatef(0.0f, BEAK_Y, bz);
    setMaterial(BEAK_COL, 95.0f);
    drawEllipsoid(BEAK_RX, 0.220f, BEAK_RZ);  // rx(가로), ry(세로), rz(돌출 깊이)

    // 윗/아랫부리 경계선: y=0 평면의 앞면 호
    glDisable(GL_LIGHTING);
    glColor3f(BEAK_COL[0] * 0.55f, BEAK_COL[1] * 0.45f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    const int N = 48;
    for (int i = 0; i <= N; ++i) {
        float x = BEAK_RX * (2.0f * i / N - 1.0f);          // -rx → +rx
        float z = BEAK_RZ * sqrtf(1.0f - (x/BEAK_RX)*(x/BEAK_RX));
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
    glLineWidth(1.0f);
    if (enableLighting) {
        glEnable(GL_LIGHTING);
    }

    glPopMatrix();
}

// ============================================================
// 얼굴 전체: 머리·얼굴 마스크·눈·부리.
// ============================================================
void drawFace() {
    drawHead();
    useStrongFaceLighting();
    drawEyes();
    drawBeak();
    useBaseLighting();
}
