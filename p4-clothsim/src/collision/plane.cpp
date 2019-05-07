#include "iostream"
#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../clothSimulator.h"
#include "../leak_fix.h"
#include "plane.h"

using namespace std;
using namespace CGL;

#define SURFACE_OFFSET 0.0001

void Plane::collide(PointMass &pm) {
    // TODO (Part 3): Handle collisions with planes.
    Vector3D p = pm.position;
    Vector3D lp = pm.last_position;
    if((dot(p, normal)>0 && dot(lp, normal)<=0) || (dot(p, normal)<0 && dot(lp, normal)>=0)){
        double tan_dist = dot(p-point, normal.unit()) + SURFACE_OFFSET;
        Vector3D tangent = p - (tan_dist*normal.unit());
        Vector3D correction_dist = (1-friction)*(tangent-lp);
        pm.position = lp - correction_dist;
    }
}

void Plane::render(GLShader &shader) {
    nanogui::Color color(0.7f, 0.7f, 0.7f, 1.0f);
    
    Vector3f sPoint(point.x, point.y, point.z);
    Vector3f sNormal(normal.x, normal.y, normal.z);
    Vector3f sParallel(normal.y - normal.z, normal.z - normal.x,
                       normal.x - normal.y);
    sParallel.normalize();
    Vector3f sCross = sNormal.cross(sParallel);
    
    MatrixXf positions(3, 4);
    MatrixXf normals(3, 4);
    
    positions.col(0) << sPoint + 2 * (sCross + sParallel);
    positions.col(1) << sPoint + 2 * (sCross - sParallel);
    positions.col(2) << sPoint + 2 * (-sCross + sParallel);
    positions.col(3) << sPoint + 2 * (-sCross - sParallel);
    
    normals.col(0) << sNormal;
    normals.col(1) << sNormal;
    normals.col(2) << sNormal;
    normals.col(3) << sNormal;
    
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
