#include "piplup.h"

// 색상을 설정하는 작은 헬퍼 함수.
// glColor3f는 0.0f~1.0f 범위의 RGB 값을 받는다.
// 예: (1, 0, 0)은 빨강, (0, 1, 0)은 초록, (0, 0, 1)은 파랑.
void setColor(float r, float g, float b) {
    glColor3f(r, g, b);
}

// 구를 비율만 다르게 늘려서 타원체처럼 그리는 함수.
// GLUT에는 타원체 함수가 따로 없으므로 glutSolidSphere를 그리기 전에
// glScalef로 좌표계를 늘려서 타원체를 만든다.
void drawEllipsoid(float sx, float sy, float sz, float radius) {
    // 현재 변환 행렬을 저장한다.
    // 이 함수 안에서 적용한 scale이 다른 물체에 영향을 주지 않게 하기 위해 필요하다.
    glPushMatrix();

    // 이후에 그려지는 물체의 x, y, z 크기를 각각 sx, sy, sz만큼 늘린다.
    glScalef(sx, sy, sz);

    // 원래는 반지름 radius인 구지만, 위에서 scale을 걸었기 때문에 타원체처럼 보인다.
    // 두 번째/세 번째 인자는 구를 얼마나 촘촘하게 나눌지 정한다.
    // 값이 클수록 둥글지만 연산량도 늘어난다.
    glutSolidSphere(radius, 40, 40);

    // 저장해둔 변환 행렬을 복원한다.
    // 이 줄이 없으면 다음에 그리는 모든 물체도 같은 scale을 물려받는다.
    glPopMatrix();
}

// GLUT의 glutSolidCone은 기본적으로 z축 방향으로 원뿔을 그린다.
// 여기서는 머리 위 장식처럼 y축 위쪽으로 서 있는 원뿔을 만들고 싶어서
// 먼저 회전시킨 뒤 원뿔을 그린다.
void drawConeBetweenY(float radius, float height) {
    glPushMatrix();

    // z축을 향하던 원뿔을 y축 방향으로 세우기 위한 회전.
    // x축 기준 -90도 회전하면 z 방향이 y 방향으로 바뀐다.
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    // radius는 밑면 반지름, height는 원뿔 높이.
    // 24, 12는 원뿔 표면을 나누는 세그먼트 수다.
    glutSolidCone(radius, height, 24, 12);

    glPopMatrix();
}

// 펭도리 부리를 직접 만든 메시로 그린다.
// 메시란 "정점 목록(vertices)"과 "면 목록(faces)"으로 3D 형태를 정의하는 방식이다.
// 여기서는 삼각뿔에 가까운 아주 단순한 부리만 직접 메시로 만들었다.
void drawBeakMesh() {
    // 부리를 구성하는 4개의 점.
    // 앞의 세 점은 얼굴에 붙는 삼각형 밑면이고,
    // 마지막 점은 앞으로 튀어나온 꼭짓점이다.
    Vec3 vertices[] = {
        { 0.0f,  0.20f,  0.55f}, // 0번: 밑면 위쪽 점
        {-0.25f, -0.12f,  0.55f}, // 1번: 밑면 왼쪽 아래 점
        { 0.25f, -0.12f,  0.55f}, // 2번: 밑면 오른쪽 아래 점
        { 0.0f,  0.02f,  1.05f}  // 3번: 앞으로 튀어나온 부리 끝점
    };

    // faces는 "어떤 정점 3개를 이어서 삼각형 하나를 만들지"를 나타낸다.
    // 예를 들어 {0, 1, 3}은 vertices[0], vertices[1], vertices[3]을 이어
    // 삼각형 한 면을 만든다는 뜻이다.
    int faces[][3] = {
        {0, 1, 3}, // 왼쪽 윗면
        {1, 2, 3}, // 아래쪽 면
        {2, 0, 3}, // 오른쪽 윗면
        {0, 2, 1}  // 얼굴에 붙는 삼각형 밑면
    };

    // 아래부터는 faces 배열을 순회하면서 삼각형들을 실제로 그리는 부분이다.
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 4; ++i) {
        // faces[i]에 적힌 정점 번호를 실제 Vec3 좌표로 꺼낸다.
        Vec3 a = vertices[faces[i][0]];
        Vec3 b = vertices[faces[i][1]];
        Vec3 c = vertices[faces[i][2]];

        // 삼각형 하나를 구성하는 세 점을 OpenGL에 넘긴다.
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
    }
    glEnd();
}

// 펭도리 날개도 간단한 직접 메시로 만든다.
// 네 점으로 이루어진 얇은 삼각뿔 형태라서 납작한 날개처럼 보인다.
void drawWingMesh() {
    // 0, 1, 2번 점은 납작한 삼각형의 외곽이다.
    // 3번 점은 살짝 앞으로 나온 점이라서 완전한 평면보다 입체감이 생긴다.
    Vec3 vertices[] = {
        { 0.00f,  0.55f, 0.0f},  // 0번: 위쪽 점
        {-0.32f, -0.40f, 0.0f},  // 1번: 왼쪽 아래 점
        { 0.22f, -0.58f, 0.0f},  // 2번: 오른쪽 아래 점
        { 0.00f,  0.10f, 0.22f}  // 3번: 앞쪽으로 살짝 나온 중심점
    };

    // 부리와 마찬가지로 정점 번호 3개를 묶어 삼각형 면을 만든다.
    int faces[][3] = {
        {0, 1, 3}, // 왼쪽 면
        {1, 2, 3}, // 아래 면
        {2, 0, 3}, // 오른쪽 면
        {0, 2, 1}  // 뒤쪽의 납작한 삼각형 면
    };

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 4; ++i) {
        Vec3 a = vertices[faces[i][0]];
        Vec3 b = vertices[faces[i][1]];
        Vec3 c = vertices[faces[i][2]];

        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
    }
    glEnd();
}

// 펭도리 전체 모델을 그리는 함수.
// 각 부위는 대부분 원점 기준으로 만든 기본 도형을
// glTranslatef, glRotatef, glScalef로 배치해서 조립한다.
void drawPiplup() {
    // 1. 몸통
    // 파란 구를 y축으로 길게, z축으로 조금 납작하게 만들어 몸통 타원체로 사용한다.
    setColor(0.12f, 0.45f, 0.95f);
    drawEllipsoid(1.05f, 1.25f, 0.82f);

    // 2. 배의 흰색 부분
    // 몸통 앞쪽(z 양수 방향)에 납작한 흰 타원체를 붙인다.
    glPushMatrix();
    glTranslatef(0.0f, -0.12f, 0.62f);
    setColor(0.88f, 0.96f, 1.0f);
    drawEllipsoid(0.62f, 0.82f, 0.18f);
    glPopMatrix();

    // 3. 머리
    // 몸통 위쪽에 진한 파란색 구를 배치한다.
    glPushMatrix();
    glTranslatef(0.0f, 1.28f, 0.02f);
    setColor(0.04f, 0.17f, 0.55f);
    drawEllipsoid(0.93f, 0.93f, 0.83f);
    glPopMatrix();

    // 4. 왼쪽 눈
    // 얼굴 앞쪽(z 양수 방향)에 작은 검은 타원체를 붙인다.
    glPushMatrix();
    glTranslatef(-0.28f, 1.40f, 0.72f);
    setColor(0.02f, 0.02f, 0.025f);
    drawEllipsoid(0.13f, 0.18f, 0.08f);
    glPopMatrix();

    // 5. 오른쪽 눈
    glPushMatrix();
    glTranslatef(0.28f, 1.40f, 0.72f);
    setColor(0.02f, 0.02f, 0.025f);
    drawEllipsoid(0.13f, 0.18f, 0.08f);
    glPopMatrix();

    // 6. 왼쪽 눈 하이라이트
    // 눈 위에 아주 작은 흰색 타원체를 올려서 눈이 덜 평면적으로 보이게 한다.
    glPushMatrix();
    glTranslatef(-0.23f, 1.47f, 0.79f);
    setColor(1.0f, 1.0f, 1.0f);
    drawEllipsoid(0.04f, 0.05f, 0.025f);
    glPopMatrix();

    // 7. 오른쪽 눈 하이라이트
    glPushMatrix();
    glTranslatef(0.33f, 1.47f, 0.79f);
    setColor(1.0f, 1.0f, 1.0f);
    drawEllipsoid(0.04f, 0.05f, 0.025f);
    glPopMatrix();

    // 8. 부리
    // 직접 만든 삼각뿔 메시를 얼굴 중앙 앞쪽에 배치한다.
    glPushMatrix();
    glTranslatef(0.0f, 1.13f, 0.50f);
    setColor(1.0f, 0.78f, 0.18f);
    drawBeakMesh();
    glPopMatrix();

    // 9. 왼쪽 날개
    // 날개 메시를 몸통 왼쪽에 배치한 뒤 회전시켜 자연스럽게 아래로 내려가게 한다.
    glPushMatrix();
    glTranslatef(-0.93f, 0.18f, 0.08f);
    glRotatef(28.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-18.0f, 0.0f, 1.0f, 0.0f);
    setColor(0.04f, 0.17f, 0.55f);
    drawWingMesh();
    glPopMatrix();

    // 10. 오른쪽 날개
    // 같은 drawWingMesh를 재사용한다.
    // glScalef(-1, 1, 1)을 사용하면 x축 기준으로 좌우 반전되어 반대쪽 날개가 된다.
    glPushMatrix();
    glTranslatef(0.93f, 0.18f, 0.08f);
    glScalef(-1.0f, 1.0f, 1.0f);
    glRotatef(28.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-18.0f, 0.0f, 1.0f, 0.0f);
    setColor(0.04f, 0.17f, 0.55f);
    drawWingMesh();
    glPopMatrix();

    // 11. 머리 위 돌기 3개
    // for문으로 i=-1,0,1 세 번 반복해서 왼쪽/중앙/오른쪽 돌기를 만든다.
    for (int i = -1; i <= 1; ++i) {
        glPushMatrix();

        // i 값에 따라 x 위치를 조금씩 바꾼다.
        glTranslatef(i * 0.22f, 2.18f, 0.02f);

        // 양옆 돌기는 살짝 기울이고, 중앙 돌기는 거의 똑바로 세운다.
        glRotatef(i * 20.0f, 0.0f, 0.0f, 1.0f);

        setColor(0.18f, 0.62f, 1.0f);
        drawConeBetweenY(0.11f, 0.45f);

        glPopMatrix();
    }

    // 12. 왼쪽 발
    // 노란 타원체를 몸통 아래 앞쪽에 배치한다.
    glPushMatrix();
    glTranslatef(-0.45f, -1.18f, 0.42f);
    setColor(1.0f, 0.78f, 0.18f);
    drawEllipsoid(0.38f, 0.16f, 0.28f);
    glPopMatrix();

    // 13. 오른쪽 발
    glPushMatrix();
    glTranslatef(0.45f, -1.18f, 0.42f);
    setColor(1.0f, 0.78f, 0.18f);
    drawEllipsoid(0.38f, 0.16f, 0.28f);
    glPopMatrix();
}

// 바닥 격자를 그린다.
// 3D 장면에서 모델이 공간 안에 있다는 느낌을 주고,
// 회전할 때 깊이감을 확인하기 쉽게 만든다.
void drawGround() {
    // 격자선은 조명 영향을 받으면 색이 의도와 다르게 보일 수 있으므로 잠시 조명을 끈다.
    glDisable(GL_LIGHTING);

    setColor(0.25f, 0.25f, 0.25f);

    // GL_LINES는 두 정점마다 선 하나를 그린다.
    glBegin(GL_LINES);
    for (int i = -5; i <= 5; ++i) {
        // z 방향으로 길게 뻗은 선.
        glVertex3f((float)i, -1.35f, -5.0f);
        glVertex3f((float)i, -1.35f, 5.0f);

        // x 방향으로 길게 뻗은 선.
        glVertex3f(-5.0f, -1.35f, (float)i);
        glVertex3f(5.0f, -1.35f, (float)i);
    }
    glEnd();

    // 펭도리 모델은 다시 조명 영향을 받아야 하므로 조명을 다시 켠다.
    glEnable(GL_LIGHTING);
}
