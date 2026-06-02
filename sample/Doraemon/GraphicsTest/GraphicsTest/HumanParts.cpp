#include <glut.h>
#include "HumanParts.h"
#include "texture_loader.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float doraR = 0.0078f;
float doraG = 0.6706f;
float doraB = 0.9373f;

void renderEllipticBulgedCylinder(float majorRadius, float minorRadius, float height, int slices = 64, int stacks = 32) {
    float domeHeight = height * 0.2f;
    float bodyHeight = height - 2.0f * domeHeight;
    if (bodyHeight < 0.0f) bodyHeight = 0.0f;

    GLUquadric* quad = gluNewQuadric();

    // 중앙 원기둥: Z축 기준, -bodyHeight/2
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -bodyHeight / 2.0f);
    glScalef(majorRadius, minorRadius, 1.0f); // 단면 타원형
    gluCylinder(quad, 1.0f, 1.0f, bodyHeight, slices, stacks);
    glPopMatrix();

    // 아래 부풀린 덮개
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -bodyHeight / 2.0f);
    glScalef(majorRadius, minorRadius, domeHeight);
    glutSolidSphere(1.0f, slices, stacks);
    glPopMatrix();

    // 위 부풀린 덮개
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, bodyHeight / 2.0f);
    glScalef(majorRadius, minorRadius, domeHeight);
    glutSolidSphere(1.0f, slices, stacks);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

void renderTaperedEllipticBulgedCylinder(
    float bottomMajor, float bottomMinor,
    float topMajor, float topMinor,
    float height, int slices = 64, int stacks = 32)
{
    float domeHeight = height * 0.2f;
    float bodyHeight = height - 2.0f * domeHeight;
    if (bodyHeight < 0.0f) bodyHeight = 0.0f;

    GLUquadric* quad = gluNewQuadric();

    // 중앙 원기둥 (tapering)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -bodyHeight / 2.0f);
    glScalef(1.0f, 1.0f, 1.0f); // 원통 축 기준
    gluCylinder(quad, bottomMajor, topMajor, bodyHeight, slices, stacks); // Z축 방향
    glPopMatrix();

    // 아래 돔
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -bodyHeight / 2.0f);
    glScalef(bottomMajor, bottomMinor, domeHeight);
    glutSolidSphere(1.0f, slices, stacks);
    glPopMatrix();

    // 위 돔
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, bodyHeight / 2.0f);
    glScalef(topMajor, topMinor, domeHeight);
    glutSolidSphere(1.0f, slices, stacks);
    glPopMatrix();

    gluDeleteQuadric(quad);
}


void renderHemisphere(float radius, int slices = 32, int stacks = 16) {
    for (int i = 0; i < stacks; ++i) {
        float theta1 = (float)i / stacks * (M_PI / 2.0f); // 0 ~ PI/2
        float theta2 = (float)(i + 1) / stacks * (M_PI / 2.0f);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float phi = (float)j / slices * 2.0f * M_PI; // 0 ~ 2PI

            float x1 = cosf(phi) * sinf(theta1);
            float y1 = sinf(phi) * sinf(theta1);
            float z1 = cosf(theta1);

            float x2 = cosf(phi) * sinf(theta2);
            float y2 = sinf(phi) * sinf(theta2);
            float z2 = cosf(theta2);

            glNormal3f(x2, y2, z2);
            glVertex3f(radius * x2, radius * y2, radius * z2);

            glNormal3f(x1, y1, z1);
            glVertex3f(radius * x1, radius * y1, radius * z1);
        }
        glEnd();
    }
}



// 기본 부분별 렌더링 함수들
//몸체 렌더링 - 루트노드
void renderDoraBody() {
    glColor3f(doraR, doraG, doraB); // 도라에몽 파란색
    glPushMatrix();
    glScalef(1.1f, 1.3f, 1.0f);
    glutSolidSphere(1.0, 30, 30);
    glPopMatrix();
}

// 배 렌더링
void renderDoraBodyParts() {
    glColor3f(0.9f, 0.9f, 0.9f); 
    glPushMatrix();
    glScalef(0.75f, 0.8f, 0.34f);
    glutSolidSphere(1.0, 30, 30);
    glPopMatrix();
}

// 머리 렌더링
void renderDoraHead() {
    glColor3f(doraR, doraG, doraB);
    glutSolidSphere(1.4, 30, 30);
}

// 목줄 렌더링
void renderDoraNeck() {
    glColor3f(0.8f, 0.3f, 0.3f);

    glPushMatrix();
    glTranslatef(0.0f, -0.15f, 0.0f); // 머리 아래 위치
    glRotatef(1.5f, 1.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Y축을 중심으로 도넛이 XZ 평면에 놓이도록
    glutSolidTorus(0.06f, 1.0f, 20, 40);
    glPopMatrix();
}

// 방울 렌더링
void renderDoraRing() {
    glColor3f(0.9f, 0.9f, 0.3f);
    glScalef(1.0f, 1.0f, 0.8f);
    glutSolidSphere(0.225, 20, 20);
}

// 코 렌더링 - 꼬리 렌더링에 재사용
void renderDoraNose() {
    glColor3f(0.9f, 0.17f, 0.17f);
    glScalef(1.0f, 1.0f, 0.8f);
    glutSolidSphere(0.165, 20, 20);
}

// 얼굴 렌더링, 텍스처링
void renderDoraFace() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, faceTextureId);

    glColor3f(1.0f, 1.0f, 1.0f);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricNormals(quad, GLU_SMOOTH);

    gluSphere(quad, 1.25, 40, 40);  // 반지름 1.2짜리 텍스처 얼굴

    gluDeleteQuadric(quad);
    glDisable(GL_TEXTURE_2D);
}

// 눈 흰자 렌더링
void renderDoraEyes() {
    glColor3f(0.9f, 0.9f, 0.9f); // 눈 색
    glScalef(1.25f, 1.45f, 0.25f);
    glutSolidSphere(0.255, 20, 20);
}

// 눈 검은자 렌더링
void renderDoraEyeBall() {
    glColor3f(0.1f, 0.1f, 0.1f); // 눈 색
    glScalef(1.0f, 1.1f, 0.25f);
    glutSolidSphere(0.055, 20, 20);
}

// 팔 렌더링 , 끝으로 갈수록 좁아지는 양끝이 볼록한 원기둥
void renderDoraArmUpgraded() {
    glColor3f(doraR, doraG, doraB);  // 도라에몽 팔 색상
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);  // Z축 정렬 → Y축 방향
    renderTaperedEllipticBulgedCylinder(
        0.30f, 0.30f,   // 아래쪽: 어깨
        0.20f, 0.20f,   // 위쪽: 손목
        1.25f);          // 전체 길이
    glPopMatrix();
}

// 손 렌더링
void renderDorahandUpgraded() {
    glColor3f(0.9f, 0.9f, 0.9f);

    glPushMatrix();
    glScalef(0.365f, 0.385f, 0.35f);
    glutSolidSphere(0.75f, 20, 20);;
    glPopMatrix();
}

// 다리 렌더링
void renderDoraLegUpgraded() {
    glColor3f(doraR, doraG, doraB);

    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); 
    renderEllipticBulgedCylinder(0.5f, 0.45f, 1.3f);
    glPopMatrix();
}

// 발 렌더링
void renderDoraFootUpgraded() {
    glColor3f(0.9f, 0.9f, 0.9f);

    glPushMatrix();
    glScalef(0.60f, 0.35f, 0.65f);
    glutSolidSphere(1.0f, 20, 20);;
    glPopMatrix();
}

// 주머니 렌더링
void renderDoraPocket() {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f); // 흰색
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 0.2f, 1.25f); // 납작한 형태
    renderHemisphere(0.5f, 30, 15);  // 반구 호출
    glPopMatrix();
}



// buildHuman 함수
SceneNode* buildHuman() {
    SceneNode* body = new SceneNode();
    body->setRenderFunction(renderDoraBody);
    body->setTranslation(0.0f, 0.0f, 0.0f);

    SceneNode* bodyParts = new SceneNode();
    bodyParts->setRenderFunction(renderDoraBodyParts);
    bodyParts->setTranslation(0.0f, -0.08f, 0.695f);
    body->addChild(bodyParts);

    SceneNode* neck = new SceneNode();
    neck->setRenderFunction(renderDoraNeck);
    neck->setTranslation(0.0f, 0.692f, 0.07f);  
    body->addChild(neck);
    SceneNode* ring = new SceneNode();
    ring->setRenderFunction(renderDoraRing);
    ring->setTranslation(0.0f, -0.28f, 1.0f);
    neck->addChild(ring);

    SceneNode* head = new SceneNode();
    head->setRenderFunction(renderDoraHead);
    head->setTranslation(0.0f, 1.6f, 0.0f);
    body->addChild(head);
    SceneNode* tail = new SceneNode();
    tail->setRenderFunction(renderDoraNose);
    tail->setRotation(-25.0f, 0.0f, 0.0f);
    tail->setTranslation(0.0f, -0.8f, -0.85f);
    body->addChild(tail);

    SceneNode* face = new SceneNode();
    face->setRenderFunction(renderDoraFace);
    face->setTranslation(0.0f, -0.15f, 0.255f); // 머리 앞쪽
    face->setRotation(-20.0f, 0.0f, 0.0f);
    head->addChild(face);
    SceneNode* leftEye = new SceneNode();
    leftEye->setRenderFunction(renderDoraEyes);
    leftEye->setTranslation(0.245f, 0.15f, 1.198f);
    leftEye->setRotation(0.0f, 8.0f, -8.0f);
    face->addChild(leftEye);
    SceneNode* rightEye = new SceneNode();
    rightEye->setRenderFunction(renderDoraEyes);
    rightEye->setTranslation(-0.245f, 0.15f, 1.198f);
    rightEye->setRotation(0.0f, -8.0f, 8.0f);
    face->addChild(rightEye);
    SceneNode* nose = new SceneNode();
    nose->setRenderFunction(renderDoraNose);
    nose->setTranslation(0.0f, -0.15f, 1.198f);
    face->addChild(nose);

    SceneNode* leftEyeBall = new SceneNode();
    leftEyeBall->setRenderFunction(renderDoraEyeBall);
    leftEyeBall->setTranslation(-0.08f, -0.08f, 0.25f);
    leftEye->addChild(leftEyeBall);
    SceneNode* rightEyeBall = new SceneNode();
    rightEyeBall->setRenderFunction(renderDoraEyeBall);
    rightEyeBall->setTranslation(0.08f, -0.08f, 0.25f);
    rightEye->addChild(rightEyeBall);


    SceneNode* leftArm = new SceneNode();
    leftArm->setRenderFunction(renderDoraArmUpgraded);
    leftArm->setTranslation(-1.1f, 0.35f, 0.0f);
    leftArm->setRotation(0.0f, 0.0f, 90.0f);
    body->addChild(leftArm);
    SceneNode* leftHand = new SceneNode();
    leftHand->setRenderFunction(renderDorahandUpgraded);
    leftHand->setTranslation(0.0f, 0.75f, 0.0f);
    leftArm->addChild(leftHand);

    SceneNode* rightArm = new SceneNode();
    rightArm->setRenderFunction(renderDoraArmUpgraded);
    rightArm->setTranslation(1.1f, 0.35f, 0.0f);
    rightArm->setRotation(0.0f, 0.0f, -90.0f);
    body->addChild(rightArm);
    SceneNode* rightHand = new SceneNode();
    rightHand->setRenderFunction(renderDorahandUpgraded);
    rightHand->setTranslation(0.0f, 0.75f, 0.0f);
    rightArm->addChild(rightHand);

    SceneNode* leftLeg = new SceneNode();
    leftLeg->setRenderFunction(renderDoraLegUpgraded);
    leftLeg->setTranslation(-0.5f, -1.0f, 0.0f);
    body->addChild(leftLeg);
    SceneNode* leftFeet = new SceneNode();
    leftFeet->setRenderFunction(renderDoraFootUpgraded);
    leftFeet->setTranslation(0.0f, -0.5f, 0.0f);
    leftLeg->addChild(leftFeet);

    SceneNode* rightLeg = new SceneNode();
    rightLeg->setRenderFunction(renderDoraLegUpgraded);
    rightLeg->setTranslation(0.5f, -1.0f, 0.0f);
    body->addChild(rightLeg);
    SceneNode* rightFeet = new SceneNode();
    rightFeet->setRenderFunction(renderDoraFootUpgraded);
    rightFeet->setTranslation(0.0f, -0.5f, 0.0f);
    rightLeg->addChild(rightFeet);

    SceneNode* pocket = new SceneNode();
    pocket->setRenderFunction(renderDoraPocket);
    pocket->setTranslation(0.0f, -0.0f, 1.0f);
    body->addChild(pocket);

    return body;
}