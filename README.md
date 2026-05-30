# 3D Virtual Human Modeling & Visualization

OpenGL을 사용한 3D 가상 인체 모델링 및 시각화 프로젝트
(성주가 원하는 캐릭터 아무거나...)

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
