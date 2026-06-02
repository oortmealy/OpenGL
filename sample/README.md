# OpenGL 캐릭터 모델링 과제 분석 정리

## 과제 목표

현재 샘플들은 GLUT/OpenGL 기반으로 캐릭터를 구성하고, 마우스/키보드 입력으로 화면을 회전하거나 확대/축소하는 구조를 공통적으로 사용한다. 이번 과제는 캐릭터 하나를 선택해 3D로 모델링하고, 사용자가 화면을 회전, 확대, 축소할 수 있게 만드는 것이 핵심이다.

## 샘플 코드에서 공통으로 보이는 구조

### 1. 프로그램 초기화

대부분의 샘플은 `main()`에서 GLUT 창을 만들고 콜백 함수를 등록한다.

필요 기능:

- `glutInit`, `glutInitDisplayMode`, `glutInitWindowSize`, `glutCreateWindow`로 창 생성
- `glutDisplayFunc`, `glutReshapeFunc`, `glutKeyboardFunc`, `glutMouseFunc`, `glutMotionFunc` 등록
- 깊이 테스트, 조명, 텍스처 사용 여부 초기화

만들 함수:

- `int main(int argc, char** argv)`
- `void init()`

### 2. 화면 그리기

모든 샘플은 `display()` 또는 `myDisplay()` 같은 렌더링 콜백에서 화면을 지우고, 카메라/모델 변환을 적용한 뒤 캐릭터 부품을 그린다.

필요 기능:

- `glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)`
- `glLoadIdentity()`
- `gluLookAt()` 또는 `glTranslatef()`로 카메라 위치 설정
- `glRotatef()`로 전체 캐릭터 회전
- 캐릭터 부위별 그리기 함수 호출
- `glutSwapBuffers()`로 더블 버퍼 출력

만들 함수:

- `void display()`
- `void drawCharacter()`

### 3. 창 크기 변경 처리

샘플들은 창 크기가 바뀔 때 `reshape()`에서 뷰포트와 투영 행렬을 다시 설정한다.

필요 기능:

- `glViewport(0, 0, w, h)`
- `glMatrixMode(GL_PROJECTION)`
- `gluPerspective()`로 원근 투영 설정
- `glMatrixMode(GL_MODELVIEW)`로 복귀

만들 함수:

- `void reshape(int w, int h)`

### 4. 캐릭터 모델링

공통적으로 캐릭터는 머리, 몸통, 팔, 손, 다리, 발, 얼굴 장식 같은 부위로 쪼개서 만든다. 각 부위는 `glPushMatrix()`와 `glPopMatrix()` 사이에서 위치 이동, 회전, 크기 조절을 적용한 뒤 구, 원기둥, 원뿔, 토러스 등을 그린다.

자주 쓰는 도형:

- `glutSolidSphere()` 또는 `gluSphere()` : 머리, 눈, 손, 발
- `gluCylinder()` : 팔, 다리, 목, 몸통
- `glutSolidCone()` : 머리카락, 귀, 장식
- `glutSolidTorus()` : 목걸이, 띠, 링 형태
- `glBegin(GL_QUADS)`, `GL_TRIANGLE_STRIP`, `GL_LINE_STRIP` : 치마, 배경, 선 장식, 곡선 표현

만들 함수 예시:

- `void drawHead()`
- `void drawFace()`
- `void drawBody()`
- `void drawLeftArm()`
- `void drawRightArm()`
- `void drawLeftLeg()`
- `void drawRightLeg()`
- `void drawHands()`
- `void drawFeet()`
- `void drawAccessories()`
- `void drawCharacter()`

### 5. 변환 함수 사용 패턴

캐릭터 부품을 정확한 위치에 놓기 위해 모든 샘플이 행렬 스택을 사용한다.

필수 패턴:

```cpp
glPushMatrix();
glTranslatef(x, y, z);
glRotatef(angle, axisX, axisY, axisZ);
glScalef(scaleX, scaleY, scaleZ);
// 도형 그리기
glPopMatrix();
```

주의할 점:

- 부위 하나를 그릴 때마다 `glPushMatrix()`와 `glPopMatrix()`를 짝지어야 한다.
- 전체 캐릭터 회전/확대는 `display()`에서 먼저 적용하고, 각 부위 위치 변환은 부위별 함수 안에서 적용하는 구조가 가장 단순하다.
- 좌우 팔/다리는 같은 함수에 위치값만 다르게 주거나, 좌우 전용 함수로 나눌 수 있다.

### 6. 화면 회전

샘플들은 두 가지 방식으로 회전을 구현한다.

첫 번째 방식은 마우스 드래그 좌표 차이로 회전값을 누적하는 방식이다.

필요 전역 변수:

```cpp
float rotateX = 0.0f;
float rotateY = 0.0f;
int lastX = 0;
int lastY = 0;
bool isDragging = false;
```

만들 함수:

- `void mouse(int button, int state, int x, int y)`
- `void motion(int x, int y)`

구현 방식:

```cpp
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isDragging = true;
        lastX = x;
        lastY = y;
    } else if (state == GLUT_UP) {
        isDragging = false;
    }
}

void motion(int x, int y) {
    if (isDragging) {
        rotateY += x - lastX;
        rotateX += y - lastY;
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}
```

`display()`에서는 다음처럼 적용한다.

```cpp
glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
```

### 7. 확대/축소

샘플들은 `zoom`, `zoomZ` 같은 값을 두고 키보드 또는 마우스 휠로 값을 바꾼다.

방법 1: 카메라 거리를 바꾸기

```cpp
float zoom = 5.0f;

gluLookAt(
    0.0f, 0.0f, zoom,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f
);
```

방법 2: 모델 스케일을 바꾸기

```cpp
float scale = 1.0f;
glScalef(scale, scale, scale);
```

이번 과제에는 방법 1이 더 안정적이다. 캐릭터 비율은 그대로 두고 카메라만 앞뒤로 움직이기 때문이다.

만들 함수:

- `void keyboard(unsigned char key, int x, int y)`
- 선택: `void mouseWheel(int wheel, int direction, int x, int y)`

구현 방식:

```cpp
void keyboard(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A') zoom -= 0.3f; // 확대
    if (key == 'z' || key == 'Z') zoom += 0.3f; // 축소

    if (zoom < 1.5f) zoom = 1.5f;
    if (zoom > 20.0f) zoom = 20.0f;

    glutPostRedisplay();
}
```

### 8. 조명과 깊이 테스트

3D 캐릭터가 입체적으로 보이려면 깊이 테스트와 조명이 필요하다.

만들 함수:

- `void setupLighting()`

필요 설정:

```cpp
glEnable(GL_DEPTH_TEST);
glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);
glEnable(GL_COLOR_MATERIAL);
glEnable(GL_NORMALIZE);
```

조명 예시:

```cpp
void setupLighting() {
    GLfloat lightPos[] = { 1.0f, 2.0f, 5.0f, 1.0f };
    GLfloat ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glEnable(GL_COLOR_MATERIAL);
}
```

### 9. 텍스처 사용

일부 샘플은 `stb_image.h`, `GLAUX`, TGA 로더 등을 사용해 얼굴, 옷, 바닥 이미지를 입힌다. 과제 필수 조건이 아니면 색상만으로 모델링해도 되지만, 캐릭터 특징을 살리려면 얼굴이나 옷에 텍스처를 쓰는 것이 좋다.

만들 함수:

- 선택: `GLuint loadTexture(const char* filename)`

필요 기능:

- 이미지 로드
- `glGenTextures`, `glBindTexture`
- `glTexImage2D`
- `glTexParameteri`
- 그릴 때 `glEnable(GL_TEXTURE_2D)`와 `glBindTexture()` 사용

## 최종적으로 구현하면 좋은 함수 목록

최소 구현:

```cpp
void init();
void setupLighting();
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

void drawCharacter();
void drawHead();
void drawFace();
void drawBody();
void drawLeftArm();
void drawRightArm();
void drawLeftLeg();
void drawRightLeg();
```

선택 구현:

```cpp
GLuint loadTexture(const char* filename);
void mouseWheel(int wheel, int direction, int x, int y);
void drawHands();
void drawFeet();
void drawHair();
void drawAccessories();
void drawGround();
```

## 과제 수행 절차

실제로 과제를 진행할 때는 아래 순서대로 하면 된다.

### 1. 캐릭터 선택

먼저 만들 캐릭터를 하나 정한다. 너무 복잡한 캐릭터보다 구, 원기둥, 원뿔 같은 기본 도형으로 특징을 표현할 수 있는 캐릭터가 좋다.

선택 기준:

- 머리, 몸통, 팔, 다리 구조가 단순한가
- 색상만으로 캐릭터 특징이 잘 드러나는가
- 눈, 코, 입, 머리카락, 옷, 장식 같은 포인트를 몇 개 만들 수 있는가
- 텍스처 없이도 어느 정도 알아볼 수 있는가

예시:

- 도라에몽: 둥근 머리, 파란 몸, 흰 얼굴, 빨간 코, 방울
- 찰리 브라운: 둥근 머리, 노란 티셔츠, 검은 지그재그 무늬
- 헬로키티: 둥근 얼굴, 귀, 리본, 수염
- 아톰: 큰 머리, 뾰족한 머리카락, 단순한 몸통

### 2. 캐릭터를 부품으로 나누기

선택한 캐릭터를 한 번에 만들려고 하지 말고 부품 단위로 나눈다.

기본 부품:

- 머리
- 얼굴 요소: 눈, 코, 입, 귀
- 몸통
- 왼팔, 오른팔
- 왼손, 오른손
- 왼다리, 오른다리
- 왼발, 오른발
- 캐릭터 특징 장식: 머리카락, 리본, 방울, 옷 무늬 등

이 단계에서 각 부품을 어떤 도형으로 만들지 정한다.

예시:

- 머리: `glutSolidSphere`
- 몸통: `gluCylinder` 또는 `glutSolidSphere`를 `glScalef`로 변형
- 팔/다리: `gluCylinder`
- 손/발: 작은 `glutSolidSphere`
- 머리카락/귀: `glutSolidCone`
- 목걸이/링: `glutSolidTorus`

### 3. 기본 프로젝트 뼈대 만들기

캐릭터를 그리기 전에 OpenGL 프로그램의 기본 구조부터 만든다.

먼저 구현할 함수:

```cpp
int main(int argc, char** argv);
void init();
void display();
void reshape(int w, int h);
```

확인할 것:

- 창이 정상적으로 뜨는가
- 배경색이 보이는가
- 창 크기를 바꿔도 프로그램이 죽지 않는가

### 4. 기본 모델링 먼저 하기

처음부터 색이나 텍스처를 고민하지 말고 흰색이나 회색으로 큰 형태만 만든다.

작업 순서:

1. 머리와 몸통을 먼저 배치한다.
2. 팔과 다리를 추가한다.
3. 손과 발을 추가한다.
4. 얼굴 요소를 추가한다.
5. 캐릭터 특징 장식을 추가한다.

이 단계의 목표는 "색이 없어도 대략 어떤 캐릭터인지 형태가 보이는 것"이다.

### 5. 위치와 비율 조정

모델링 후에는 각 부품의 위치와 크기를 조정한다.

주로 조정할 함수:

- `glTranslatef(x, y, z)`: 부품 위치
- `glRotatef(angle, x, y, z)`: 부품 방향
- `glScalef(x, y, z)`: 부품 비율

확인할 것:

- 머리와 몸통이 떨어져 있지 않은가
- 팔과 다리가 몸통에 자연스럽게 붙어 있는가
- 좌우 대칭이 필요한 부품은 균형이 맞는가
- 캐릭터가 화면 중앙에 보이는가

### 6. 색 입히기

형태와 비율이 어느 정도 맞으면 각 부품에 색을 입힌다. 색은 도형을 그리기 직전에 `glColor3f()`로 지정한다.

예시:

```cpp
glColor3f(1.0f, 0.8f, 0.6f);
glutSolidSphere(0.5f, 30, 30);
```

주의할 점:

- 색은 `0.0f`부터 `1.0f` 사이 값으로 지정한다.
- 조명을 켰다면 `glEnable(GL_COLOR_MATERIAL)`도 같이 설정해야 색이 잘 보인다.
- 텍스처를 사용할 때는 흰색 `glColor3f(1.0f, 1.0f, 1.0f)`을 사용하는 것이 안전하다.

### 7. 회전 기능 구현

마우스 왼쪽 버튼을 누른 상태로 드래그하면 캐릭터가 회전하도록 만든다.

필요한 전역 변수:

```cpp
float rotateX = 0.0f;
float rotateY = 0.0f;
int lastX = 0;
int lastY = 0;
bool isDragging = false;
```

구현할 함수:

```cpp
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
```

`display()`에는 누적된 회전값을 적용한다.

```cpp
glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
```

### 8. 확대/축소 기능 구현

키보드나 마우스 휠로 카메라 거리를 바꾼다. 가장 단순한 방식은 `zoom` 값을 두고 `gluLookAt()`의 카메라 z 위치에 사용하는 것이다.

필요한 전역 변수:

```cpp
float zoom = 5.0f;
```

구현할 함수:

```cpp
void keyboard(unsigned char key, int x, int y);
```

예시:

```cpp
if (key == 'a') zoom -= 0.3f; // 확대
if (key == 'z') zoom += 0.3f; // 축소
```

확대/축소는 너무 가까워지거나 멀어지지 않도록 최소/최대값을 제한한다.

### 9. 조명과 깊이감 확인

모델이 평면처럼 보이면 조명과 깊이 테스트를 확인한다.

필수 설정:

```cpp
glEnable(GL_DEPTH_TEST);
glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);
glEnable(GL_COLOR_MATERIAL);
glEnable(GL_NORMALIZE);
```

확인할 것:

- 앞에 있는 부품이 뒤 부품을 제대로 가리는가
- 캐릭터가 너무 어둡거나 너무 밝지 않은가
- 회전했을 때 입체감이 보이는가

### 10. 선택 기능 추가

시간이 남으면 텍스처, 배경, 바닥, 간단한 애니메이션을 추가한다.

선택 작업:

- 얼굴이나 옷에 텍스처 입히기
- 바닥 또는 배경 그리기
- 캐릭터 이름을 화면에 표시하기
- `idle()`에서 작은 움직임 추가하기

단, 필수 기능인 모델링, 회전, 확대/축소가 먼저 완성되어야 한다.

### 11. 제출 전 최종 확인

제출 전에 아래 항목을 직접 실행해서 확인한다.

- 프로그램이 실행되는가
- 캐릭터가 화면 중앙에 보이는가
- 마우스 드래그로 회전되는가
- 키보드로 확대/축소되는가
- 창 크기를 바꿔도 화면이 깨지지 않는가
- 조명과 깊이 테스트가 적용되어 3D처럼 보이는가
- 소스 파일과 이미지 파일 경로가 제출 환경에서도 맞는가

## 추천 구현 순서

1. `main()`, `init()`, `display()`, `reshape()`만 먼저 만들고 빈 화면이 뜨는지 확인한다.
2. `setupLighting()`과 `glEnable(GL_DEPTH_TEST)`를 추가한다.
3. `drawCharacter()`에서 머리와 몸통만 먼저 그린다.
4. 팔, 다리, 손, 발을 부위별 함수로 추가한다.
5. 얼굴, 머리카락, 옷, 장식처럼 선택한 캐릭터의 특징을 추가한다.
6. 마우스 드래그 회전(`mouse`, `motion`)을 구현한다.
7. 키보드 확대/축소(`keyboard`)를 구현한다.
8. 필요하면 텍스처와 배경을 추가한다.

## 과제 체크리스트

- [ ] 캐릭터 하나를 정했다.
- [ ] 머리, 몸통, 팔, 다리 등 기본 부위를 3D 도형으로 만들었다.
- [ ] 선택한 캐릭터를 알아볼 수 있는 특징을 넣었다.
- [ ] `glPushMatrix()`와 `glPopMatrix()`를 부위별로 올바르게 사용했다.
- [ ] 마우스 드래그로 화면 또는 캐릭터가 회전한다.
- [ ] 키보드 또는 마우스 휠로 확대/축소가 된다.
- [ ] 창 크기 변경 시 화면 비율이 깨지지 않는다.
- [ ] 깊이 테스트와 조명으로 3D 입체감이 보인다.

## 샘플별 참고 포인트

- `Doraemon/GraphicsTest/GraphicsTest`: `SceneNode`를 사용해 부품을 계층적으로 붙이는 구조가 좋다.
- `atom/.../main.cpp`: 부위별 `draw_*` 함수가 많아 캐릭터를 함수 단위로 나누는 예시로 좋다.
- `kitty/TanningKitty/.../tanningKitty.cpp`: 마우스 드래그 회전과 키보드 확대/축소 구조가 단순해서 참고하기 좋다.
- `charlie brown/.../main.cpp`: 캐릭터 특징, 배경, 텍스처, 조명까지 포함한 완성형 예시다.
- `parkmyeongsu/ComputerGraphics_HW02`: 별도 카메라 클래스로 회전/이동을 처리하는 예시다.
- `IU/project-GL-master/person.cpp`: `createSphere`, `createCylinder`, `createCircle`처럼 기본 도형 helper 함수를 만들어 재사용하는 방식이 좋다.

## 결론

이번 과제에서 꼭 만들어야 하는 핵심은 `초기화`, `화면 그리기`, `창 크기 처리`, `입력 처리`, `캐릭터 부위별 모델링` 함수다. 구현은 복잡한 카메라 클래스보다 전역 변수 `rotateX`, `rotateY`, `zoom`을 두고 `display()`에서 `gluLookAt`, `glRotatef`를 적용하는 방식이 가장 빠르고 안정적이다.
