#pragma once

// 얼굴 전체 (머리·얼굴 마스크·눈·부리)를 그린다.
void drawFace();

// 머리 텍스처 초기화.
// frontPath: "reference/head.png", backPath: "reference/head_back.png"
void initHeadTexture(const char* frontPath, const char* backPath);
