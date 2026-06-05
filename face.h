#pragma once

// 얼굴 전체 (머리·얼굴 마스크·눈·부리)를 그린다.
void drawFace();

// 머리 텍스처 초기화.
// path 예시: "reference/head.png"
void initHeadTexture(const char* path);
