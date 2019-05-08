#ifndef COLLISIONOBJECT_CTRIANGLE_H
#define COLLISIONOBJECT_CTRIANGLE_H

#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "collisionObject.h"

using namespace nanogui;
using namespace CGL;
using namespace std;

struct cTriangle : public CollisionObject {
public:
    cTriangle(const Vector3D &pointA, const Vector3D &pointB, const Vector3D &pointC, const Vector3D &normal, double friction)
    : pointA(pointA), pointB(pointB), pointC(pointC), normal(normal), friction(friction) {}
    
    void render(GLShader &shader);
    void collide(PointMass &pm);
    
    Vector3D pointA;
    Vector3D pointB;
    Vector3D pointC;
    Vector3D normal;
    
    double friction;
};

#endif /* COLLISIONOBJECT_CTRIANGLE_H */
