#include "iostream"
#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../clothSimulator.h"
#include "../leak_fix.h"
#include "ctriangle.h"

using namespace std;
using namespace CGL;

#define SURFACE_OFFSET 0.0001

void cTriangle::collide(PointMass &pm) {
    return;
}

void cTriangle::render(GLShader &shader) {
    nanogui::Color color(0.7f, 0.7f, 0.7f, 1.0f);
//    Vector3f sPoint(point.x, point.y, point.z);
//    Vector3f sNormal(normal.x, normal.y, normal.z);
//    Vector3f sParallel(normal.y - normal.z, normal.z - normal.x,
//                       normal.x - normal.y);
//    sParallel.normalize();
//    Vector3f sCross = sNormal.cross(sParallel);
    Vector3f pA(pointA.x, pointA.y, pointA.z);
    Vector3f pB(pointB.x, pointB.y, pointB.z);
    Vector3f pC(pointC.x, pointC.y, pointC.z);
    Vector3f last(0.0, 0.0, 0.0);
    Vector3f n(normal.x, normal.y, normal.z);

    MatrixXf positions(3, 4);
    MatrixXf normals(3, 4);

    positions.col(0) << pA;
    positions.col(1) << pB;
    positions.col(2) << pC;
    positions.col(3) << last;
    
    normals.col(0) << n;
    normals.col(1) << n;
    normals.col(2) << n;
    normals.col(3) << n;

    if (shader.uniform("u_color", false) != -1) {
        shader.setUniform("u_color", color);
    }
    shader.uploadAttrib("in_position", positions);
    if (shader.attrib("in_normal", false) != -1) {
        shader.uploadAttrib("in_normal", normals);
    }

    shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);
#ifdef LEAK_PATCH_ON
    shader.freeAttrib("in_position");
    if (shader.attrib("in_normal", false) != -1) {
        shader.freeAttrib("in_normal");
    }
#endif
}
