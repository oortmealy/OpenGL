// macOS에서는 OpenGL/GLUT 헤더 위치가 Windows/Linux와 다르다.
// __APPLE__ 매크로가 정의되어 있으면 macOS용 헤더를 포함한다.
#ifdef __APPLE__
// macOS에서 OpenGL이 deprecated 되었다는 경고를 숨긴다.
// 수업/과제용 구형 OpenGL 코드를 쓸 때 불필요한 경고를 줄이기 위한 설정이다.
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
// Windows/Linux 계열에서는 보통 이 경로로 GLUT 헤더를 포함한다.
#include <GL/glut.h>
#endif

// fmaxf, fminf 같은 float 전용 수학 함수를 사용하기 위한 헤더.
#include <cmath>
// exit(0)을 사용하기 위한 헤더.
#include <cstdlib>

// 카메라와 모델 회전에 필요한 전역 상태값들.
// GLUT의 콜백 함수(display, keyboard, mouse 등)는 전역 함수 형태로 등록되므로,
// 여러 콜백에서 공유해야 하는 값은 전역 변수로 두는 방식이 간단하다.

// 카메라가 모델에서 얼마나 떨어져 있는지 나타낸다.
// 값이 작아질수록 zoom-in, 커질수록 zoom-out처럼 보인다.
float cameraDistance = 7.0f;

// 마우스 드래그로 조절되는 모델의 X축 회전 각도.
// 위아래로 드래그하면 이 값이 바뀐다.
float rotateX = 15.0f;

// 마우스 드래그로 조절되는 모델의 Y축 회전 각도.
// 좌우로 드래그하면 이 값이 바뀐다.
float rotateY = -25.0f;

// 현재 마우스 왼쪽 버튼을 누른 채 드래그 중인지 저장한다.
bool isDragging = false;

// 직전 마우스 위치를 저장한다.
// 현재 위치와 직전 위치의 차이를 이용해서 회전량을 계산한다.
int lastMouseX = 0;
int lastMouseY = 0;

// 메시(mesh)를 직접 만들 때 사용할 간단한 3차원 좌표 구조체.
// x, y, z는 각각 오른쪽/위쪽/앞뒤 방향 좌표라고 생각하면 된다.
struct Vec3 {
    float x;
    float y;
    float z;
};

// 색상을 설정하는 작은 헬퍼 함수.
// glColor3f는 0.0f~1.0f 범위의 RGB 값을 받는다.
// 예: (1, 0, 0)은 빨강, (0, 1, 0)은 초록, (0, 0, 1)은 파랑.
void setColor(float r, float g, float b) {
    glColor3f(r, g, b);
}

// 구를 비율만 다르게 늘려서 타원체처럼 그리는 함수.
// GLUT에는 타원체 함수가 따로 없으므로 glutSolidSphere를 그리기 전에
// glScalef로 좌표계를 늘려서 타원체를 만든다.
void drawEllipsoid(float sx, float sy, float sz, float radius = 1.0f) {
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

// 화면을 다시 그려야 할 때 GLUT가 호출하는 콜백 함수.
// OpenGL 렌더링의 핵심 흐름은 보통 display 함수 안에 들어간다.
void display() {
    // 색상 버퍼와 깊이 버퍼를 모두 지운다.
    // 색상 버퍼: 이전 프레임의 픽셀 색
    // 깊이 버퍼: 어떤 물체가 앞에 있고 뒤에 있는지 판단하는 z-depth 정보
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 이제부터 카메라/모델 변환을 설정하겠다는 뜻이다.
    glMatrixMode(GL_MODELVIEW);

    // 이전 프레임의 변환이 남아 있지 않도록 단위 행렬로 초기화한다.
    glLoadIdentity();

    // 카메라 설정.
    // 첫 3개: 카메라 위치
    // 다음 3개: 카메라가 바라보는 지점
    // 마지막 3개: 카메라의 위쪽 방향
    gluLookAt(
        0.0f, 1.1f, cameraDistance,
        0.0f, 0.35f, 0.0f,
        0.0f, 1.0f, 0.0f
    );

    // 조명의 위치를 매 프레임 설정한다.
    // w=1.0이면 위치가 있는 점광원처럼 동작한다.
    GLfloat lightPosition[] = {3.0f, 5.0f, 5.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // 바닥 격자를 먼저 그린다.
    drawGround();

    // 모델 전체 회전.
    // 마우스 드래그로 바뀐 rotateX, rotateY 값을 적용한다.
    glPushMatrix();
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
    drawPiplup();
    glPopMatrix();

    // GLUT_DOUBLE을 사용했기 때문에 화면 뒤쪽 버퍼에 다 그린 뒤,
    // 앞쪽 버퍼와 교체해서 깜빡임 없이 보여준다.
    glutSwapBuffers();
}

// 창 크기가 처음 만들어지거나 사용자가 창 크기를 바꿀 때 호출된다.
// 여기서 화면 비율에 맞는 원근 투영을 다시 계산한다.
void reshape(int width, int height) {
    // height가 0이면 width/height 계산에서 0으로 나누기 문제가 생긴다.
    if (height == 0) {
        height = 1;
    }

    // OpenGL이 그릴 영역을 창 전체로 설정한다.
    glViewport(0, 0, width, height);

    // 이제부터 투영 행렬을 설정한다.
    // 투영 행렬은 3D 공간을 2D 화면에 어떻게 보여줄지 결정한다.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 원근 투영 설정.
    // 45도: 카메라 시야각
    // width/height: 창의 가로세로 비율
    // 0.1: 카메라에 너무 가까운 물체를 잘라내는 거리
    // 100: 너무 먼 물체를 잘라내는 거리
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);

    // 다시 모델뷰 행렬로 돌려놓는다.
    // 이후 display에서 카메라/모델 변환을 설정하기 때문이다.
    glMatrixMode(GL_MODELVIEW);
}

// 키보드 입력 처리 콜백.
// key에는 눌린 키의 ASCII 값이 들어온다.
void keyboard(unsigned char key, int, int) {
    // ESC 키의 ASCII 값은 27이다.
    // ESC를 누르면 프로그램을 종료한다.
    if (key == 27) {
        exit(0);
    }

    // a 키: 카메라를 모델에 더 가깝게 이동시켜 zoom-in처럼 보이게 한다.
    if (key == 'a' || key == 'A') {
        // 너무 가까워지면 모델 내부로 들어갈 수 있으므로 최소 거리를 3.0으로 제한한다.
        cameraDistance = fmaxf(3.0f, cameraDistance - 0.3f);
    } else if (key == 'z' || key == 'Z') {
        // z 키: 카메라를 뒤로 빼서 zoom-out처럼 보이게 한다.
        // 너무 멀어지면 모델이 작아져 보이지 않으므로 최대 거리를 12.0으로 제한한다.
        cameraDistance = fminf(12.0f, cameraDistance + 0.3f);
    }

    // 카메라 거리가 바뀌었으니 화면을 다시 그리라고 요청한다.
    glutPostRedisplay();
}

// 마우스 버튼을 누르거나 뗄 때 호출되는 콜백.
void mouse(int button, int state, int x, int y) {
    // 왼쪽 마우스 버튼만 드래그 회전에 사용한다.
    if (button == GLUT_LEFT_BUTTON) {
        // 버튼을 누르면 GLUT_DOWN, 떼면 GLUT_UP이 들어온다.
        isDragging = (state == GLUT_DOWN);

        // 드래그 시작점 또는 마지막 위치를 저장한다.
        lastMouseX = x;
        lastMouseY = y;
    }
}

// 마우스를 누른 채 움직일 때 호출되는 콜백.
// 현재 위치와 이전 위치의 차이만큼 모델 회전값을 바꾼다.
void motion(int x, int y) {
    // 왼쪽 버튼을 누른 상태가 아니면 회전하지 않는다.
    if (!isDragging) {
        return;
    }

    // x 방향 이동량은 y축 회전으로 연결한다.
    // 마우스를 오른쪽으로 끌면 모델이 좌우로 돌아간다.
    rotateY += (x - lastMouseX) * 0.5f;

    // y 방향 이동량은 x축 회전으로 연결한다.
    // 마우스를 위아래로 끌면 모델이 위아래로 기울어진다.
    rotateX += (y - lastMouseY) * 0.5f;

    // 다음 motion 호출에서 이동량을 계산하기 위해 현재 위치를 저장한다.
    lastMouseX = x;
    lastMouseY = y;

    // 회전값이 바뀌었으므로 화면을 다시 그린다.
    glutPostRedisplay();
}

// OpenGL 초기 설정을 모아둔 함수.
// 창을 만든 뒤, 렌더링을 시작하기 전에 한 번 호출한다.
void init() {
    // 배경색을 어두운 회색으로 설정한다.
    // 인자는 RGBA이며, 마지막 alpha는 투명도지만 여기서는 크게 의미 없다.
    glClearColor(0.08f, 0.09f, 0.11f, 1.0f);

    // 깊이 테스트를 켠다.
    // 이 옵션이 없으면 뒤에 있는 면이 앞의 면 위에 그려지는 문제가 생긴다.
    glEnable(GL_DEPTH_TEST);

    // 기본 조명을 켠다.
    // 조명이 있어야 구/타원체의 입체감이 더 잘 보인다.
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // glColor3f로 지정한 색상이 조명 계산에 반영되게 한다.
    // 이 설정이 없으면 조명을 켰을 때 glColor3f 색이 기대처럼 안 보일 수 있다.
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // ambient는 전체적으로 깔리는 약한 빛이다.
    // 값이 너무 낮으면 그림자진 부분이 완전히 어둡게 보일 수 있다.
    GLfloat ambient[] = {0.28f, 0.28f, 0.28f, 1.0f};

    // diffuse는 표면 방향과 빛 방향에 따라 밝기가 달라지는 주된 빛이다.
    GLfloat diffuse[] = {0.85f, 0.85f, 0.85f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
}

// 프로그램 시작점.
// GLUT 초기화, 창 생성, 콜백 등록을 한 뒤 이벤트 루프에 들어간다.
int main(int argc, char** argv) {
    // GLUT 라이브러리를 초기화한다.
    // 명령줄 인자를 GLUT가 필요한 만큼 처리할 수 있게 argc/argv를 넘긴다.
    glutInit(&argc, argv);

    // 디스플레이 모드 설정.
    // GLUT_DOUBLE: 더블 버퍼링 사용
    // GLUT_RGB: RGB 색상 사용
    // GLUT_DEPTH: 깊이 버퍼 사용
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // 창 크기와 위치 설정.
    glutInitWindowSize(800, 700);
    glutInitWindowPosition(100, 100);

    // 실제 창 생성. 문자열은 창 제목으로 표시된다.
    glutCreateWindow("OpenGL - Simple 3D Piplup");

    // OpenGL 상태 초기화.
    init();

    // GLUT 콜백 등록.
    // 특정 이벤트가 발생하면 아래 함수들이 자동으로 호출된다.
    glutDisplayFunc(display);   // 화면을 그려야 할 때 호출
    glutReshapeFunc(reshape);   // 창 크기가 바뀔 때 호출
    glutKeyboardFunc(keyboard); // 키보드 입력이 들어올 때 호출
    glutMouseFunc(mouse);       // 마우스 버튼 상태가 바뀔 때 호출
    glutMotionFunc(motion);     // 마우스를 누른 채 움직일 때 호출

    // GLUT 이벤트 루프 시작.
    // 이 함수는 프로그램이 종료될 때까지 내부에서 계속 실행된다.
    glutMainLoop();

    return 0;
}
