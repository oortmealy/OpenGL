# 3D Virtual Human Modeling & Visualization

OpenGL을 사용한 3D 가상 인체 모델링 및 시각화 프로젝트
(성주가 원하는 캐릭터 아무거나...)

## 목표 설계 수준

### 구조 설계
- `Vector3` 등 수학 유틸리티 클래스를 직접 구현하여 좌표계 관리
- `MyCamera` 클래스로 카메라 위치/방향/회전을 캡슐화
- `mySphere`, `myCapsule`, `myDrawCube` 등 도형별 커스텀 함수로 모델링 추상화
- `DrawGuide` (좌표축 시각화), `DrawNet` (격자 바닥) 등 보조 시각화 요소 포함

### 모델링
- `glutSolidSphere`, `gluCylinder`, `GL_QUADS/GL_TRIANGLE_FAN` 조합으로 인체 각 부위 구성
- 구/타원체/캡슐/원기둥/반구 등 다양한 형태를 파라미터로 제어 가능하도록 구현
- 모든 부위는 원점 기준으로 제작 후 `glTranslatef`/`glRotatef`로 조립

### 시각화
- BMP 텍스처 2~3장 이상 적용 (신체 부위별 개별 텍스처)
- `GL_LIGHTING` + `glLightfv`를 이용한 조명(ambient/diffuse) 설정
- `glScalef`로 구를 타원체로 변형하는 등 기본 도형의 응용

### 인터랙션
- `gluPerspective`의 FOV 또는 카메라 거리 조절 방식으로 zoom 구현
- 마우스 드래그 시 theta/phi 기반 구면 좌표 계산으로 자연스러운 3D 회전

## 요구사항

### 구현 기능
- [ ] 3D 인체 모델링 (가능한 사실적으로)
- [ ] 텍스처 매핑을 통한 실감나는 시각화
- [ ] 마우스 드래깅으로 3D rotation
- [ ] `a` 키: zoom-in
- [ ] `z` 키: zoom-out

### 제약 조건
- 모든 모델링 요소는 3D 객체
- 모든 요소는 원점 기준 좌표계로 제작 후 `glTranslatef`, `glRotatef`로 배치
- 미리 제작된 메쉬 파일 사용 금지

### 사용 가능한 OpenGL 함수
```
glutSolidCube(size)
glutSolidSphere(radius, slices, stacks)
glutSolidTorus(innerRadius, outerRadius, nsides, rings)
glutSolidCone(base, height, slices, stacks)
glutSolidTetrahedron()
glutSolidIcosahedron()
```
그 외 수학식으로 모델링 가능한 메쉬 (직육면체, 실린더 등) 직접 구현 가능

## 프로젝트 구조

```
OpenGL/
├── main.cpp          # 메인 진입점
├── .vscode/          # VSCode 빌드 설정 (macOS)
│   ├── tasks.json
│   ├── launch.json
│   └── c_cpp_properties.json
├── build/            # 컴파일 결과물 (git 제외)
└── README.md
```

## 빌드 및 실행

### VSCode
`Cmd+Shift+B` → Build OpenGL

### 터미널
```bash
clang++ -std=c++17 *.cpp -framework OpenGL -framework GLUT -o build/Debug/outDebug
./build/Debug/outDebug
```

## 제출
- `학번.zip`: Visual Studio 프로젝트 파일 및 `.cpp`, `.h`
- `학번.ppt`: 함수 설명 및 실행 사례
- 실행 결과 동영상 (캠타시아, 곰캠 등)

### 제출 전 점검 사항
- [ ] ppt에 배운 점, 느낀 점, 추가로 하고 싶었는데 못한 점 작성했나 확인
- [ ] 데모 동영상 촬영
- [ ] ppt 첫장에 무엇을 만들었는 지 빡 집어넣기
