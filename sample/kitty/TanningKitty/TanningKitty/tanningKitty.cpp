#include <glut.h> 

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdbool.h>  

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

GLuint plushTexture;
GLuint feltTexture;

void loadTexture(const char* filename, GLuint* textureID);

float angleX = 0.0f, angleY = 0.0f;
int lastX, lastY;
bool isDragging = false;
float zoomZ = 4.0f;  // 카메라 z축 거리 (기본값)

// 꽃 장식
void drawRedFlower() {
    glPushMatrix();

    // 회전
    glRotatef(-40, 1, 0, 0);
    glRotatef(15, 0, 1, 0);
    glRotatef(50, 0, 0, 1);

    // 위치 이동
    glTranslatef(0.3f, -0.1f, 1.0f);

    for (int i = 0; i < 5; i++) {
        float angle = i * 72.0f;
        float rad = angle * 3.14159f / 180.0f;
        float dx = 0.18f * cos(rad);
        float dy = 0.18f * sin(rad);

        glPushMatrix();
        glTranslatef(dx, dy, 0.0f);
        glRotatef(angle, 0, 0, 1);
        glScalef(1.4f, 1.0f, 1.0f);
        glColor3f(0.9f, 0.0f, 0.0f);  // 빨간색
        glutSolidSphere(0.13, 20, 20);
        glPopMatrix();
    }

    glColor3f(1.0f, 1.0f, 0.2f);  // 노란 중심
    glutSolidSphere(0.08, 20, 20);
    glPopMatrix();
}

void drawWhiteFlower(float tx, float ty, float tz, int rxangle, int ryangle, int rzangle) {
    glPushMatrix();

    // 1. 위치 조정
    glTranslatef(tx, ty, tz);

    // 2. 회전 조정 
    glRotatef(rxangle, 1, 0, 0);
    glRotatef(ryangle, 0, 1, 0);
    glRotatef(rzangle, 0, 0, 1);

    // 3. 꽃잎 5개 (타원형)
    for (int i = 0; i < 5; i++) {
        float angle = i * 72.0f;
        float rad = angle * 3.14159f / 180.0f;
        float dx = 0.18f * cos(rad);
        float dy = 0.18f * sin(rad);

        glPushMatrix();
        glTranslatef(dx, dy, 0.0f);        // 중심 기준
        glRotatef(angle, 0, 0, 1);         // 중심을 향하도록
        glScalef(1.2f, 0.9f, 1.0f);        // 뚱뚱한 타원형
        glColor3f(1.0f, 1.0f, 1.0f);       // White
        glutSolidSphere(0.13, 20, 20);     // 꽃잎 하나
        glPopMatrix();
    }

    // 중심 구체
    glColor3f(1.0f, 1.0f, 0.2f);
    glutSolidSphere(0.08, 20, 20);        // 중심 구체

    glPopMatrix();
}

// 키티 얼굴 + 꽃 장식 
void drawKittyFace()
{
    // 얼굴 (타원형)
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, plushTexture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1.4f, 1.4f, 1.4f);
    //glRotatef(90, 1, 0, 0); 
    glRotatef(180, 0, 1, 0);
    glScalef(1.2f, 1.0f, 1.0f);

    GLUquadric* faceQuad = gluNewQuadric();
    gluQuadricTexture(faceQuad, GL_TRUE);
    gluSphere(faceQuad, 0.95, 50, 50);
    gluDeleteQuadric(faceQuad);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // 귀 - 왼쪽
    glPushMatrix();
    glTranslatef(-0.63f, 0.57f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glRotatef(-37, 0, 1, 0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, plushTexture);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);

    GLUquadric* leftEar = gluNewQuadric();
    gluQuadricTexture(leftEar, GL_TRUE);
    gluCylinder(leftEar, 0.55, 0.0, 0.6, 20, 20); // 원뿔
    gluDisk(leftEar, 0.0, 0.55, 20, 1);           // 밑면
    gluDeleteQuadric(leftEar);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // 귀 - 오른쪽
    glPushMatrix();
    glTranslatef(0.63f, 0.57f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glRotatef(37, 0, 1, 0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, plushTexture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);

    GLUquadric* rightEar = gluNewQuadric();
    gluQuadricTexture(rightEar, GL_TRUE);
    gluCylinder(rightEar, 0.55, 0.0, 0.6, 20, 20);
    gluDisk(rightEar, 0.0, 0.55, 20, 1);
    gluDeleteQuadric(rightEar);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // 눈 (왼쪽)
    glPushMatrix();
    glColor3f(0.35f, 0.23f, 0.12f);
    glTranslatef(-0.4f, 0.2f, 0.9f);
    glScalef(0.08f, 0.10f, 0.05f);
    glutSolidSphere(1.0, 30, 30);
    glPopMatrix();

    // 눈 (오른쪽)
    glPushMatrix();
    glColor3f(0.35f, 0.23f, 0.12f);
    glTranslatef(0.4f, 0.2f, 0.9f);
    glScalef(0.08f, 0.10f, 0.05f);
    glutSolidSphere(1.0, 30, 30);
    glPopMatrix();

    // 코 (노란 타원)
    glPushMatrix();
    glColor3f(1.0f, 0.9f, 0.1f);
    glTranslatef(0.0f, -0.06f, 0.95f);
    glScalef(0.18f, 0.12f, 0.05f);
    glutSolidSphere(0.7, 30, 30);
    glPopMatrix();

    // 수염 - 왼쪽
    glColor3f(0.35f, 0.23f, 0.12f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.65f, 0.2f, 0.75f); glVertex3f(-1.1f, 0.3f, 0.75f);
    glVertex3f(-0.65f, 0.1f, 0.75f); glVertex3f(-1.1f, 0.1f, 0.75f);
    glVertex3f(-0.65f, 0.0f, 0.75f); glVertex3f(-1.1f, -0.1f, 0.75f);
    glEnd();

    // 수염 - 오른쪽
    glColor3f(0.35f, 0.23f, 0.12f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(0.65f, 0.2f, 0.75f); glVertex3f(1.1f, 0.3f, 0.75f);
    glVertex3f(0.65f, 0.1f, 0.75f); glVertex3f(1.1f, 0.1f, 0.75f);
    glVertex3f(0.65f, 0.0f, 0.75f); glVertex3f(1.1f, -0.2f, 0.75f);
    glEnd();

    // 꽃 장식
    drawRedFlower();
    drawWhiteFlower(0.12f, 0.88f, 0.43f, -60, 0, -70);
    drawWhiteFlower(-0.2f, 0.88f, 0.43f, -60, 0, -46);
    drawWhiteFlower(-0.60f, 0.76f, 0.58f, -40, -25, 10);
}

void drawPinkShell() {
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();

    for (int i = 0; i < 5; i++) {
        float angle = i * 72.0f;
        float rad = angle * 3.14159f / 180.0f;
        float dx = 0.1f * cos(rad);
        float dy = 0.1f * sin(rad);

        glPushMatrix();
        glTranslatef(dx, dy, 0.0f);
        glRotatef(angle, 0, 0, 1);
        glScalef(1.2f, 0.8f, 1.0f);
        glColor3f(1.0f, 0.6f, 0.8f);  // 연핑크
        glutSolidSphere(0.07, 20, 20);
        glPopMatrix();
    }

    // 바느질 자국 3개
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    float stitchHeight = 0.12f;
    float stitchWidth = 0.03f;
    float stitchOffsets[3] = { -0.07f, 0.0f, 0.07f };
    float stitchAngles[3] = { 20.0f, 0.0f, -20.0f };

    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(stitchOffsets[i], -0.03f, 0.07f);
        glRotatef(stitchAngles[i], 0, 0, 1);
        glScalef(stitchWidth, stitchHeight, 0.01f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    glPopMatrix();
}

// 조개 함수
void drawShellTop() {
    // 왼쪽 조개
    glPushMatrix();
    glTranslatef(-0.25f, -1.15f, 0.68f);
    drawPinkShell(); // 텍스처 없이
    glPopMatrix();

    // 가운데 연결 리본 (텍스처 제거)
    glDisable(GL_TEXTURE_2D);  // 텍스처 끔
    glColor3f(1.0f, 0.6f, 0.8f); // 연분홍색
    glLineWidth(6.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.15f, -1.15f, 0.70f);
    glVertex3f(0.15f, -1.15f, 0.70f);
    glEnd();

    // 오른쪽 조개
    glPushMatrix();
    glTranslatef(0.25f, -1.15f, 0.68f);
    drawPinkShell();
    glPopMatrix();

    // 몸통 연결 선 (도넛형) - 텍스처 제거
    glPushMatrix();
    glTranslatef(0.0f, -1.15f, 0.0f);
    glRotatef(90, 1, 0, 0);
    glColor3f(1.0f, 0.6f, 0.8f); // 연분홍 도넛

    glutSolidTorus(0.03f, 0.70f, 20, 60); // 텍스처 없이 도넛
    glPopMatrix();
}

// 몸통
void createBodyCylinder(GLUquadric* quad) {
    float bottomRadius = 0.8f;   // 아래가 더 넓은
    float topRadius = 0.5f;      // 위가 좁은
    float height = 1.3f;

    glPushMatrix();

    // 위치 조정 (얼굴 아래쪽으로)
    glTranslatef(0.0f, -1.6f, 0.0f);
    glRotatef(270, 1, 0, 0);

    // 몸통 색상
    glColor3f(0.9f, 0.75f, 0.6f);

    // 원기둥 옆면 (사다리꼴)
    //gluCylinder(quad, bottomRadius, topRadius, height, 30, 30);
    // 인형 질감 텍스처 적용
    glEnable(GL_TEXTURE_2D); // 텍스처 기능 켜기
    glBindTexture(GL_TEXTURE_2D, plushTexture); // 텍스처 바인딩
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluQuadricTexture(quad, GL_TRUE); // quadric 객체에 텍스처 활성화

    gluCylinder(quad, bottomRadius, topRadius, height, 30, 30);

    // 밑면, 윗면에도 텍스처 적용 가능
    glPushMatrix();
    gluDisk(quad, 0.0f, bottomRadius, 30, 1);
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0f, topRadius, 30, 1);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D); // 끝나고 끄기

    // 밑면 닫기
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    gluDisk(quad, 0.0f, bottomRadius, 30, 1); // 밑면
    glPopMatrix();

    // 윗면 닫기
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0f, topRadius, 30, 1); // 윗면
    glPopMatrix();

    glPopMatrix();
}

// 다리
void createLegCylinder(GLUquadric* quad, float xOffset) {
    float radius = 0.38f;
    float height = 0.4f;

    glPushMatrix();

    // 위치: 몸통 기준 아래쪽 + 좌우로 xOffset 만큼 이동
    glTranslatef(xOffset, -2.0f, 0.0f);  // y값은 몸통 밑에 맞춰 조정
    glRotatef(270, 1, 0, 0);             // 눕혀서 다리처럼 보이게

    glColor3f(0.9f, 0.75f, 0.6f);        // 몸 색과 동일

    glEnable(GL_TEXTURE_2D); // 텍스처 기능 켜기
    glBindTexture(GL_TEXTURE_2D, plushTexture); // 텍스처 바인딩
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluQuadricTexture(quad, GL_TRUE); // quadric 객체에 텍스처 활성화

    gluCylinder(quad, radius, radius, height, 20, 20); // 다리 원기둥

    // 밑면 닫기
    glPushMatrix();
    gluDisk(quad, 0.0f, radius, 20, 1);
    glPopMatrix();

    // 윗면 닫기
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0f, radius, 20, 1);
    glPopMatrix();

    glPopMatrix();
}

void drawLegs(GLUquadric* quad) {
    createLegCylinder(quad, -0.42f);  // 왼쪽 다리
    createLegCylinder(quad, 0.42f);  // 오른쪽 다리
}

// 치마
void drawSkirtBelt() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, feltTexture);  // 부직포 텍스처
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1.0f, 1.0f, 1.0f);  // 텍스처 색 그대로

    float innerRadius = 0.06f;
    float outerRadius = 0.85f;
    int sides = 20;
    int rings = 60;

    glPushMatrix();
    glTranslatef(0.0f, -1.38f, 0.0f);
    glRotatef(90, 1, 0, 0);  // 도넛을 세움

    for (int i = 0; i < sides; ++i) {
        float theta = (float)i / sides * 2.0f * M_PI;
        float nextTheta = (float)(i + 1) / sides * 2.0f * M_PI;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= rings; ++j) {
            float phi = (float)j / rings * 2.0f * M_PI;

            // 현재 원 둘레의 좌표
            float x0 = (outerRadius + innerRadius * cos(theta)) * cos(phi);
            float y0 = (outerRadius + innerRadius * cos(theta)) * sin(phi);
            float z0 = innerRadius * sin(theta);

            // 다음 원 둘레의 좌표
            float x1 = (outerRadius + innerRadius * cos(nextTheta)) * cos(phi);
            float y1 = (outerRadius + innerRadius * cos(nextTheta)) * sin(phi);
            float z1 = innerRadius * sin(nextTheta);

            // 텍스처 좌표 (u, v)
            float u = (float)j / rings;
            float v0 = (float)i / sides;
            float v1 = (float)(i + 1) / sides;

            glTexCoord2f(u, v0); glVertex3f(x0, y0, z0);
            glTexCoord2f(u, v1); glVertex3f(x1, y1, z1);
        }
        glEnd();
    }

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawGrassSkirt() {
    int count = 20;
    float angleStep = 360.0f / count;
    float radius = 0.85f;
    float height = 0.35f;
    float width = 0.18f;
    float depth = 0.03f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, feltTexture);  // 텍스처 바인딩
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1.0f, 1.0f, 1.0f);

    for (int i = 0; i < count; ++i) {
        float angle = i * angleStep;
        float rad = angle * 3.14159f / 180.0f;
        float x = radius * cos(rad);
        float z = radius * sin(rad);

        glPushMatrix();
        glTranslatef(x, -1.73f, z);
        glRotatef(180, 0, 1, 0);  // 몸통을 향하도록 회전

        float w = width / 2, h = height, d = depth / 2;

        glBegin(GL_QUADS);

        // 앞면
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0.0f, d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0.0f, d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, d);

        // 뒷면
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0.0f, -d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0.0f, -d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, -d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -d);

        // 좌측
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0.0f, -d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, 0.0f, d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -d);

        // 우측
        glTexCoord2f(0.0f, 0.0f); glVertex3f(w, 0.0f, -d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0.0f, d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(w, h, -d);

        // 윗면
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, h, -d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w, h, -d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, d);

        glEnd();
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
}

// HAWAII 글씨 추가
void drawHawaiiTextOnSkirt() {
    glPushMatrix();

    // 위치: 치마 왼쪽 조각 근처로 이동
    glTranslatef(-0.47f, -1.72f, 0.71f); // z: 살짝 앞
    glRotatef(90, 0, 0, 1);             // 왼쪽으로 90도 회전하여 세움
    glScalef(0.0007f, 0.0007f, 0.7f);   // 텍스트 크기 조절

    glColor3f(1.0f, 0.6f, 0.8f);        // 연분홍 텍스트
    glLineWidth(1.0f);

    const char* text = "HAWAII";
    for (int i = 0; text[i] != '\0'; i++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
    }

    glPopMatrix();
}

// 왼팔
void createLeftArm(GLUquadric* quad) {
    float bottomRadius = 0.28f;
    float topRadius = 0.1f;
    float height = 0.8f;

    glPushMatrix();

    // 팔 위치 및 회전
    glTranslatef(-0.8f, -1.4f, 0.0f);
    glRotatef(270, 1, 0, 0);
    glRotatef(30, 0, 1, 0);

    // 팔 색상
    glColor3f(0.9f, 0.75f, 0.6f);

    glEnable(GL_TEXTURE_2D); // 텍스처 기능 켜기
    glBindTexture(GL_TEXTURE_2D, plushTexture); // 텍스처 바인딩
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluQuadricTexture(quad, GL_TRUE); // quadric 객체에 텍스처 활성화

    // 팔 원기둥
    gluCylinder(quad, bottomRadius, topRadius, height, 30, 30);

    // 밑면
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    gluDisk(quad, 0.0f, bottomRadius, 30, 1);
    glPopMatrix();

    // 윗면
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0f, topRadius, 30, 1);
    glPopMatrix();

    // 손 구체: 팔 위쪽에 덧붙이기
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);

    GLUquadric* handSphere = gluNewQuadric();
    gluQuadricTexture(handSphere, GL_TRUE);
    gluSphere(handSphere, 0.28, 30, 30);
    gluDeleteQuadric(handSphere);
    glPopMatrix();

    glPopMatrix(); // 전체 팔 + 손 끝
}

// 오른팔
void createRightArm(GLUquadric* quad) {
    float bottomRadius = 0.28f;
    float topRadius = 0.25f;
    float height = 0.8f;

    glPushMatrix();

    // 팔 위치 및 회전
    glTranslatef(1.1f, -0.92f, 0.0f);
    glRotatef(90, 1, 0, 0);
    glRotatef(-72, 0, 1, 0);

    glEnable(GL_TEXTURE_2D); // 텍스처 기능 켜기
    glBindTexture(GL_TEXTURE_2D, plushTexture); // 텍스처 바인딩
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluQuadricTexture(quad, GL_TRUE); // quadric 객체에 텍스처 활성화

    // 팔 원기둥
    gluCylinder(quad, bottomRadius, topRadius, height, 30, 30);

    // 밑면
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    gluDisk(quad, 0.0f, bottomRadius, 30, 1);
    glPopMatrix();

    // 윗면
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0f, topRadius, 30, 1);
    glPopMatrix();

    // 손 구체
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    GLUquadric* handSphere = gluNewQuadric();
    gluQuadricTexture(handSphere, GL_TRUE);
    gluSphere(handSphere, 0.28, 30, 30);
    gluDeleteQuadric(handSphere);
    glPopMatrix();

    // 작은 손가락 구체
    glPushMatrix();
    glTranslatef(-0.27f, 0.0f, -0.03f);
    GLUquadric* fingerSphere = gluNewQuadric();
    gluQuadricTexture(fingerSphere, GL_TRUE);
    gluSphere(fingerSphere, 0.1f, 30, 30);
    gluDeleteQuadric(fingerSphere);
    glPopMatrix();

    glPopMatrix(); // 전체 팔 + 손 끝
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0, zoomZ, 0, 0, 0, 0, 1, 0);

    glRotatef(angleY, 1.0, 0.0, 0.0);
    glRotatef(angleX, 0.0, 1.0, 0.0);

    drawKittyFace();

    GLUquadric* quad = gluNewQuadric();
    createBodyCylinder(quad);
    drawLegs(quad);
    createLeftArm(quad);
    createRightArm(quad);
    gluDeleteQuadric(quad);
    drawShellTop();
    drawSkirtBelt();
    drawGrassSkirt();
    drawHawaiiTextOnSkirt();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    float ratio = 1.0f * w / h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, ratio, 1, 100);

    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isDragging = true;
        lastX = x;
        lastY = y;
    }
    if (state == GLUT_UP) {
        isDragging = false;
    }
}

void motion(int x, int y)
{
    if (isDragging) {
        angleX += (x - lastX);
        angleY += (y - lastY);
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

void mouseWheel(int button, int dir, int x, int y)
{
    if (dir > 0)
        zoomZ -= 0.2f; // zoom in
    else
        zoomZ += 0.2f; // zoom out

    if (zoomZ < 1.5f) zoomZ = 1.5f;
    if (zoomZ > 20.0f) zoomZ = 20.0f;

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':  // 줌 인
    case 'A':
        zoomZ -= 0.2f;
        if (zoomZ < 1.5f) zoomZ = 1.5f;
        glutPostRedisplay();
        break;
    case 'z':  // 줌 아웃
    case 'Z':
        zoomZ += 0.2f;
        if (zoomZ > 20.0f) zoomZ = 20.0f;
        glutPostRedisplay();
        break;
    }
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);  // 배경색
    loadTexture("C:/TanningKitty/TanningKitty/Texture/plush_texture.png", &plushTexture);
    loadTexture("C:/TanningKitty/TanningKitty/Texture/felt_texture.png", &feltTexture);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("3D Tanning Kitty");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);  // 키보드 입력 등록

    glutMainLoop();
    return 0;
}

// 텍스처 로딩 함수
void loadTexture(const char* filename, GLuint* textureID) {
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (!image) {
        printf("Failed to load texture: %s\n", filename);
        exit(1);
    }

    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(image);
}