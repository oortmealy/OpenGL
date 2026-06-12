#pragma once

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdlib>

// 메시를 직접 만들 때 사용할 간단한 3차원 좌표 구조체.
struct Vec3 {
    float x;
    float y;
    float z;
};

// 전역 상태 변수 선언 (정의는 main.cpp에 있음)
extern float cameraDistance;
extern float rotateX;
extern float rotateY;
extern bool  isDragging;
extern int   lastMouseX;
extern int   lastMouseY;
extern bool  enableLighting;

// --- 컬러 팔레트 (정의는 piplup.cpp에 있음) ---
extern const float BODY_DARK[3];   // 머리·등·날개 – 짙은 파랑
extern const float BODY_LIGHT[3];  // 몸통 앞면·뺨 – 하늘색
extern const float BEAK_COL[3];    // 부리·발 – 노란색
extern const float BELLY[3];       // 배 흰점·눈 흰자·얼굴 흰자
extern const float EYE_COL[3];     // 눈동자 – 검정

// --- 그리기 유틸리티 ---
void setColor(float r, float g, float b);
void setMaterial(const float col[3], float shininess);
void drawEllipsoid(float sx, float sy, float sz, float radius = 1.0f);
void drawConeBetweenY(float radius, float height);
// 수동 메시(부리·날개)의 면 법선 계산. body.cpp/face.cpp에서 공유한다.
void setFaceNormal(const Vec3& a, const Vec3& b, const Vec3& c);

// --- 모델 / 씬 ---
void drawPiplup();
void initGroundTexture(const char* path);
void drawGround();
