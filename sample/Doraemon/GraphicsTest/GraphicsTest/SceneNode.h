#pragma once
#include <vector>
#include <functional>

struct Vec3 {
    float x, y, z;
    Vec3(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f)
        : x(x_), y(y_), z(z_) {
    }
};

class SceneNode {
public:
    SceneNode();

    // 변환 설정
    void setTranslation(float x, float y, float z);
    void setRotation(float x, float y, float z);

    // 자식 추가
    void addChild(SceneNode* child);

    // 도형 렌더 함수 설정
    void setRenderFunction(const std::function<void()>& func);

    // 렌더링 실행
    void render();

private:
    Vec3 translation;
    Vec3 rotation;
    std::function<void()> renderFunc;
    std::vector<SceneNode*> children;
};
