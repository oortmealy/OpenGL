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

// --- 그리기 유틸리티 ---
void setColor(float r, float g, float b);
void drawEllipsoid(float sx, float sy, float sz, float radius = 1.0f);
void drawConeBetweenY(float radius, float height);
void drawBeakMesh();
void drawWingMesh();

// --- 모델 / 씬 ---
void drawPiplup();
void drawGround();
