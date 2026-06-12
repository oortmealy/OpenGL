#include "piplup.h"
#include "face.h"
#include "stb_image.h"

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

// false면 조명 없이 순색으로 렌더링한다.
bool enableLighting = true;

// 배경 이미지 텍스처 상태.
static GLuint backgroundTexture = 0;
static int backgroundImageWidth = 1;
static int backgroundImageHeight = 1;
static int windowWidth = 800;
static int windowHeight = 700;

// 화면 배경으로 사용할 PNG 이미지를 OpenGL 텍스처로 올린다.
void initBackgroundTexture(const char* path) {
    int w, h, ch;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(path, &w, &h, &ch, 4);
    if (!data) {
        return;
    }

    backgroundImageWidth = w;
    backgroundImageHeight = h;

    glGenTextures(1, &backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);
}

// 3D 모델을 그리기 전에 화면 전체에 배경 이미지를 먼저 렌더링한다.
void drawBackground() {
    if (!backgroundTexture || windowWidth <= 0 || windowHeight <= 0) {
        return;
    }

    float windowAspect = (float)windowWidth / (float)windowHeight;
    float imageAspect = (float)backgroundImageWidth / (float)backgroundImageHeight;

    float u0 = 0.0f;
    float u1 = 1.0f;
    float v0 = 0.0f;
    float v1 = 1.0f;

    // CSS background-size: cover처럼 비율을 유지하고 넘치는 축만 중앙 기준으로 잘라낸다.
    if (windowAspect < imageAspect) {
        float visibleWidth = windowAspect / imageAspect;
        u0 = (1.0f - visibleWidth) * 0.5f;
        u1 = u0 + visibleWidth;
    } else if (windowAspect > imageAspect) {
        float visibleHeight = imageAspect / windowAspect;
        v0 = (1.0f - visibleHeight) * 0.5f;
        v1 = v0 + visibleHeight;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glColor3f(1.0f, 1.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)windowWidth, 0.0, (GLdouble)windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBegin(GL_QUADS);
    glTexCoord2f(u0, v0); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(u1, v0); glVertex2f((float)windowWidth, 0.0f);
    glTexCoord2f(u1, v1); glVertex2f((float)windowWidth, (float)windowHeight);
    glTexCoord2f(u0, v1); glVertex2f(0.0f, (float)windowHeight);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}

// 화면을 다시 그려야 할 때 GLUT가 호출하는 콜백 함수.
// OpenGL 렌더링의 핵심 흐름은 보통 display 함수 안에 들어간다.
void display() {
    // 색상 버퍼와 깊이 버퍼를 모두 지운다.
    // 색상 버퍼: 이전 프레임의 픽셀 색
    // 깊이 버퍼: 어떤 물체가 앞에 있고 뒤에 있는지 판단하는 z-depth 정보
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawBackground();

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

    if (enableLighting) {
        // w=0.0이면 위치가 아닌 방향광으로 동작한다.
        // 점광원보다 거리별 밝기 편차가 작아서 캐릭터 전체 색이 안정적으로 보인다.
        GLfloat lightPosition[] = {-0.35f, 0.75f, 0.55f, 0.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    }

    // 산/하늘 배경 위에 3D 잔디 바닥을 먼저 그린다.
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

    windowWidth = width;
    windowHeight = height;

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

    // 기본 조명. enableLighting=false면 임시로 조명 없이 순색만 확인한다.
    if (enableLighting) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }

    // glColor3f로 지정한 색상이 조명 계산에 반영되게 한다.
    // 이 설정이 없으면 조명을 켰을 때 glColor3f 색이 기대처럼 안 보일 수 있다.
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // glScalef로 구를 늘릴 때 법선 벡터 길이가 함께 변하므로,
    // 조명 계산 전에 자동으로 정규화하도록 설정한다.
    glEnable(GL_NORMALIZE);

    // ambient는 전체적으로 깔리는 빛이다.
    // 방향광만 쓰면 그림자진 부분이 너무 어두워져서 약간 넉넉하게 둔다.
    GLfloat modelAmbient[] = {0.18f, 0.18f, 0.18f, 1.0f};
    GLfloat ambient[] = {0.36f, 0.36f, 0.36f, 1.0f};

    // diffuse는 표면 방향과 빛 방향에 따라 밝기가 달라지는 주된 빛이다.
    GLfloat diffuse[] = {0.72f, 0.72f, 0.72f, 1.0f};

    // specular는 반짝이는 하이라이트다. 너무 세면 발/몸통 색이 날아가므로 낮춘다.
    GLfloat specular[] = {0.24f, 0.24f, 0.22f, 1.0f};

    if (enableLighting) {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, modelAmbient);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    }
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

    // 머리 텍스처 로드 (앞면 + 뒷면)
    initHeadTexture("reference/head.png", "reference/head_back.png");

    // 바닥 잔디 텍스처 로드
    initGroundTexture("reference/bg_grass.png");

    // 첨부한 배경 이미지를 화면 배경 텍스처로 로드
    initBackgroundTexture("reference/bg_pokemon.png");

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
