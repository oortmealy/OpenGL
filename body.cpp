#include "piplup.h"
#include "body.h"

static const float FOOT_LINE_COL[3] = {0.84f, 0.66f, 0.32f};

static void drawMeshTriangle(const Vec3& a, const Vec3& b, const Vec3& c) {
    setFaceNormal(a, b, c);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
}

struct TorsoRing {
    float radius;
    float y;
};

static float smoothStep01(float value) {
    float t = fminf(1.0f, fmaxf(0.0f, value));
    return t * t * (3.0f - 2.0f * t);
}

static float torsoRadiusProfile(float t) {
    float dome = sinf(3.1415926535f * t);
    float softOval = 0.74f + 0.23f * powf(fmaxf(0.0f, dome), 0.72f);
    float bottomTaper = 0.94f + 0.06f * smoothStep01(t * 1.35f);
    float topTaper = 1.0f - 0.06f * smoothStep01((t - 0.74f) / 0.26f);
    return softOval * bottomTaper * topTaper;
}

static Vec3 makeRoundedTorsoPoint(float radius, float y, float angle) {
    const float radiusX = 0.84f;
    const float radiusZ = 0.76f;

    return {
        radiusX * radius * cosf(angle),
        y,
        radiusZ * radius * sinf(angle)
    };
}

static void setTorsoNormal(float angle, float radiusSlope) {
    const float radiusX = 0.84f;
    const float radiusZ = 0.76f;
    Vec3 n = {
        cosf(angle) / radiusX,
        -radiusSlope * 0.70f,
        sinf(angle) / radiusZ
    };
    float len = sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);
    if (len > 1e-6f) {
        glNormal3f(n.x / len, n.y / len, n.z / len);
    }
}

static void drawTorsoVertex(const Vec3& p, float angle, float radiusSlope) {
    setTorsoNormal(angle, radiusSlope);
    glVertex3f(p.x, p.y, p.z);
}

static void drawRoundedTorsoBand(const TorsoRing& lower, const TorsoRing& upper, int sideSegments) {
    const float pi = 3.1415926535f;
    const float eps = 1e-5f;
    float yDelta = upper.y - lower.y;

    if (fabsf(yDelta) <= eps) {
        glNormal3f(0.0f, lower.y < 0.0f ? -1.0f : 1.0f, 0.0f);
    }

    for (int i = 0; i < sideSegments; ++i) {
        float angle0 = 2.0f * pi * (float)i / (float)sideSegments;
        float angle1 = 2.0f * pi * (float)(i + 1) / (float)sideSegments;
        float radiusSlope = fabsf(yDelta) > eps ? (upper.radius - lower.radius) / yDelta : 0.0f;

        Vec3 lower0 = makeRoundedTorsoPoint(lower.radius, lower.y, angle0);
        Vec3 lower1 = makeRoundedTorsoPoint(lower.radius, lower.y, angle1);
        Vec3 upper0 = makeRoundedTorsoPoint(upper.radius, upper.y, angle0);
        Vec3 upper1 = makeRoundedTorsoPoint(upper.radius, upper.y, angle1);

        if (fabsf(yDelta) <= eps) {
            if (lower.radius <= eps) {
                glVertex3f(lower0.x, lower0.y, lower0.z);
                glVertex3f(upper0.x, upper0.y, upper0.z);
                glVertex3f(upper1.x, upper1.y, upper1.z);
            } else if (upper.radius <= eps) {
                glVertex3f(lower0.x, lower0.y, lower0.z);
                glVertex3f(lower1.x, lower1.y, lower1.z);
                glVertex3f(upper0.x, upper0.y, upper0.z);
            }
        } else if (lower.radius <= eps) {
            drawTorsoVertex(lower0, angle0, radiusSlope);
            drawTorsoVertex(upper0, angle0, radiusSlope);
            drawTorsoVertex(upper1, angle1, radiusSlope);
        } else if (upper.radius <= eps) {
            drawTorsoVertex(lower0, angle0, radiusSlope);
            drawTorsoVertex(upper1, angle1, radiusSlope);
            drawTorsoVertex(lower1, angle1, radiusSlope);
        } else {
            drawTorsoVertex(lower0, angle0, radiusSlope);
            drawTorsoVertex(upper1, angle1, radiusSlope);
            drawTorsoVertex(lower1, angle1, radiusSlope);

            drawTorsoVertex(lower0, angle0, radiusSlope);
            drawTorsoVertex(upper0, angle0, radiusSlope);
            drawTorsoVertex(upper1, angle1, radiusSlope);
        }
    }
}

// 몸통: 위아래 폭 차이는 작고 중간만 살짝 부푼 매끈한 회전체 메시.
static void drawRoundedTorsoMesh() {
    const int sideSegments = 128;
    const int heightSegments = 56;
    const float halfHeight = 0.63f;

    TorsoRing rings[80];
    int ringCount = 0;

    rings[ringCount++] = {0.0f, -halfHeight};
    rings[ringCount++] = {torsoRadiusProfile(0.0f), -halfHeight};

    for (int i = 1; i <= heightSegments; ++i) {
        float t = (float)i / (float)heightSegments;
        float y = -halfHeight + 2.0f * halfHeight * t;
        rings[ringCount++] = {torsoRadiusProfile(t), y};
    }

    rings[ringCount++] = {0.0f, halfHeight};

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < ringCount - 1; ++i) {
        drawRoundedTorsoBand(rings[i], rings[i + 1], sideSegments);
    }
    glEnd();
}

static Vec3 makeLegPoint(float radius, float y, float angle) {
    const float radiusX = 0.18f;
    const float radiusZ = 0.20f;

    return {
        radiusX * radius * cosf(angle),
        y,
        radiusZ * radius * sinf(angle)
    };
}

static void setLegNormal(float angle, float radiusSlope) {
    const float radiusX = 0.18f;
    const float radiusZ = 0.20f;
    Vec3 n = {
        cosf(angle) / radiusX,
        -radiusSlope * 0.20f,
        sinf(angle) / radiusZ
    };
    float len = sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);
    if (len > 1e-6f) {
        glNormal3f(n.x / len, n.y / len, n.z / len);
    }
}

static void drawLegVertex(const Vec3& p, float angle, float radiusSlope) {
    setLegNormal(angle, radiusSlope);
    glVertex3f(p.x, p.y, p.z);
}

static void drawLegBand(const TorsoRing& lower, const TorsoRing& upper, int sideSegments) {
    const float pi = 3.1415926535f;
    const float eps = 1e-5f;
    float yDelta = upper.y - lower.y;

    if (fabsf(yDelta) <= eps) {
        glNormal3f(0.0f, lower.y < 0.0f ? -1.0f : 1.0f, 0.0f);
    }

    for (int i = 0; i < sideSegments; ++i) {
        float angle0 = 2.0f * pi * (float)i / (float)sideSegments;
        float angle1 = 2.0f * pi * (float)(i + 1) / (float)sideSegments;
        float radiusSlope = fabsf(yDelta) > eps ? (upper.radius - lower.radius) / yDelta : 0.0f;

        Vec3 lower0 = makeLegPoint(lower.radius, lower.y, angle0);
        Vec3 lower1 = makeLegPoint(lower.radius, lower.y, angle1);
        Vec3 upper0 = makeLegPoint(upper.radius, upper.y, angle0);
        Vec3 upper1 = makeLegPoint(upper.radius, upper.y, angle1);

        if (fabsf(yDelta) <= eps) {
            if (lower.radius <= eps) {
                glVertex3f(lower0.x, lower0.y, lower0.z);
                glVertex3f(upper0.x, upper0.y, upper0.z);
                glVertex3f(upper1.x, upper1.y, upper1.z);
            } else if (upper.radius <= eps) {
                glVertex3f(lower0.x, lower0.y, lower0.z);
                glVertex3f(lower1.x, lower1.y, lower1.z);
                glVertex3f(upper0.x, upper0.y, upper0.z);
            }
        } else if (lower.radius <= eps) {
            drawLegVertex(lower0, angle0, radiusSlope);
            drawLegVertex(upper0, angle0, radiusSlope);
            drawLegVertex(upper1, angle1, radiusSlope);
        } else if (upper.radius <= eps) {
            drawLegVertex(lower0, angle0, radiusSlope);
            drawLegVertex(upper1, angle1, radiusSlope);
            drawLegVertex(lower1, angle1, radiusSlope);
        } else {
            drawLegVertex(lower0, angle0, radiusSlope);
            drawLegVertex(upper1, angle1, radiusSlope);
            drawLegVertex(lower1, angle1, radiusSlope);

            drawLegVertex(lower0, angle0, radiusSlope);
            drawLegVertex(upper0, angle0, radiusSlope);
            drawLegVertex(upper1, angle1, radiusSlope);
        }
    }
}

// 다리: 몸통과 같은 매끈한 메시. 세로는 짧고 아래쪽 반지름은 더 작다.
static void drawShortLegMesh() {
    const int sideSegments = 64;
    const TorsoRing rings[] = {
        {0.0f, -0.14f},
        {0.66f, -0.14f},
        {0.82f, -0.06f},
        {1.00f,  0.10f},
        {0.0f,  0.10f}
    };
    const int ringCount = sizeof(rings) / sizeof(rings[0]);

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < ringCount - 1; ++i) {
        drawLegBand(rings[i], rings[i + 1], sideSegments);
    }
    glEnd();
}

static Vec3 makeWingPoint(float radius, float angle, float zScale) {
    const float centerX = -0.10f;
    const float centerY = -0.03f;
    const float radiusX = 0.52f;
    const float radiusY = 0.24f;
    float sinAngle = sinf(angle);
    float sideTaper = sqrtf(fabsf(sinAngle));
    const float z = zScale * sqrtf(fmaxf(0.0f, 1.0f - radius * radius));

    return {
        centerX + radiusX * radius * cosf(angle),
        centerY + radiusY * radius * sinAngle * sideTaper,
        z
    };
}

// 날개: 삼각형을 많이 쪼갠 납작한 렌즈형 메시.
// 좌우 끝으로 갈수록 얇아지게 만들어 누워 있는 뾰족한 날개 실루엣을 만든다.
static void drawWingMesh() {
    const int angleSegments = 96;
    const int radialSegments = 10;
    const float pi = 3.1415926535f;
    const float frontZ = 0.16f;
    const float backZ = -0.08f;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < angleSegments; ++i) {
        float angle0 = 2.0f * pi * (float)i / (float)angleSegments;
        float angle1 = 2.0f * pi * (float)(i + 1) / (float)angleSegments;

        for (int r = 0; r < radialSegments; ++r) {
            float radius0 = (float)r / (float)radialSegments;
            float radius1 = (float)(r + 1) / (float)radialSegments;

            Vec3 front00 = makeWingPoint(radius0, angle0, frontZ);
            Vec3 front01 = makeWingPoint(radius0, angle1, frontZ);
            Vec3 front10 = makeWingPoint(radius1, angle0, frontZ);
            Vec3 front11 = makeWingPoint(radius1, angle1, frontZ);

            Vec3 back00 = makeWingPoint(radius0, angle0, backZ);
            Vec3 back01 = makeWingPoint(radius0, angle1, backZ);
            Vec3 back10 = makeWingPoint(radius1, angle0, backZ);
            Vec3 back11 = makeWingPoint(radius1, angle1, backZ);

            if (r == 0) {
                drawMeshTriangle(front00, front10, front11);
                drawMeshTriangle(back00, back11, back10);
            } else {
                drawMeshTriangle(front00, front10, front11);
                drawMeshTriangle(front00, front11, front01);
                drawMeshTriangle(back00, back11, back10);
                drawMeshTriangle(back00, back01, back11);
            }
        }
    }
    glEnd();
}

// ============================================================
// 몸통 파트 개별 그리기 함수
// ============================================================

// 몸통 본체: 연한 하늘색 매끈한 둥근 메시.
static void drawTorso() {
    glPushMatrix();
    glTranslatef(0.0f, -0.20f, 0.0f);
    setMaterial(BODY_LIGHT, 35.0f);
    drawRoundedTorsoMesh();
    glPopMatrix();
}

// 목도리 앞쪽 얇고 넓은 타원 조각 1개.
static void drawScarfFrontPiece(float x, float angle) {
    glPushMatrix();
    glTranslatef(x, 0.40f, 0.40f);
    glRotatef(0.0f, 0.0f, angle, 0.0f);
    setMaterial(BODY_DARK, 55.0f);
    drawEllipsoid(0.350f, 0.13f, 0.50f);
    glPopMatrix();
}

// 목도리: 앞쪽 11자에 가까운 얕은 ㅅ자 타원 2개 + 뒤쪽 크고 납작한 타원 1개.
static void drawScarf() {
    glPushMatrix();
    glTranslatef(0.0f, 0.25f, -0.32f);
    glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    setMaterial(BODY_DARK, 55.0f);
    drawEllipsoid(0.8f, 0.30f, 0.9f);
    glPopMatrix();

    drawScarfFrontPiece(-0.24f, 20.0f);
    drawScarfFrontPiece(0.24f, -20.0f);
}

// 짧은 다리 1개: 몸통 아래에서 발까지 이어지는 하늘색 다리.
static void drawLeg(float x) {
    glPushMatrix();
    glTranslatef(x, -0.88f, 0.0f);
    setMaterial(BODY_LIGHT, 25.0f);
    drawShortLegMesh();
    glPopMatrix();
}

// 양쪽 다리.
static void drawLegs() {
    drawLeg(-0.43f);
    drawLeg(0.43f);
}

// 배 흰 점 1개: 좌우 대칭이라 x좌표와 회전 각도만 받아서 공용으로 그린다.
static void drawBellySpot(float x, float angle) {
    glPushMatrix();
    glTranslatef(x, -0.0f, 0.68f);
    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    setMaterial(BELLY, 30.0f);
    drawEllipsoid(0.14f, 0.14f, 0.05f);
    glPopMatrix();
}

// 배 흰 점 2개.
static void drawBellySpots() {
    drawBellySpot(-0.30f, -15.0f);
    drawBellySpot(0.30f, 15.0f);
}

// 날개 1개: x좌표와 좌우 반전 여부를 받아서 공용으로 그린다.
// 오른쪽 날개는 glScalef(-1,1,1)로 x축 좌우 반전한다.
static void drawWing(float x, bool mirror) {
    glPushMatrix();
    glTranslatef(x, 0.28f, 0.08f);
    if (mirror) glScalef(-1.0f, 1.0f, 1.0f);
    glScalef(1.00f, 1.50f, 1.25f);
    glRotatef(14.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-40.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    setMaterial(BODY_LIGHT, 60.0f);
    drawWingMesh();
    glPopMatrix();
}

// 양쪽 날개.
static void drawWings() {
    drawWing(-0.86f, false);  // 왼쪽
    drawWing(0.86f, true);    // 오른쪽 (좌우 반전)
}

static float footSurfaceTopY(float x, float z) {
    const float sx = 0.38f;
    const float sy = 0.16f;
    const float sz = 0.28f;
    float xPart = (x * x) / (sx * sx);
    float zPart = (z * z) / (sz * sz);
    return sy * sqrtf(fmaxf(0.0f, 1.0f - xPart - zPart)) + 0.006f;
}

static void drawFootToeLine(float zOffset) {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    setColor(FOOT_LINE_COL[0], FOOT_LINE_COL[1], FOOT_LINE_COL[2]);
    glLineWidth(4.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 12; ++i) {
        float t = (float)i / 12.0f;
        float x = -0.34f + 0.24f * t;
        glVertex3f(x, footSurfaceTopY(x, zOffset), zOffset);
    }
    glEnd();
    glLineWidth(1.0f);
    if (enableLighting) {
        glEnable(GL_LIGHTING);
    }
    glPopMatrix();
}

static void drawFootToeLines() {
    drawFootToeLine(-0.07f);
    drawFootToeLine(0.07f);
}

static void drawFoot(float x) {
    glPushMatrix();
    glTranslatef(x, -1.04f, 0.23f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    setMaterial(BEAK_COL, 20.0f);
    drawEllipsoid(0.38f, 0.16f, 0.28f);
    drawFootToeLines();
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
    drawScarf();
    drawBellySpots();
    drawWings();
    drawLegs();
    drawFeet();
}
