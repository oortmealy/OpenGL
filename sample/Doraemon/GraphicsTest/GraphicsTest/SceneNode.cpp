#include "SceneNode.h"
#include <glut.h>

SceneNode::SceneNode()
    : translation(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), renderFunc(nullptr) {
}

void SceneNode::setTranslation(float x, float y, float z) {
    translation = Vec3(x, y, z);
}

void SceneNode::setRotation(float x, float y, float z) {
    rotation = Vec3(x, y, z);
}

void SceneNode::setRenderFunction(const std::function<void()>& func) {
    renderFunc = func;
}

void SceneNode::addChild(SceneNode* child) {
    children.push_back(child);
}

void SceneNode::render() {
    glPushMatrix();

    // Æ®·£½ºÆû Àû¿ë
    glTranslatef(translation.x, translation.y, translation.z);
    glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);

    // ·»´õ ÇÔ¼ö ½ÇÇà
    if (renderFunc) renderFunc();

    // ÀÚ½Ä ³ëµå Àç±Í ·»´õ¸µ
    for (SceneNode* child : children) {
        if (child) child->render();
    }

    glPopMatrix();
}
