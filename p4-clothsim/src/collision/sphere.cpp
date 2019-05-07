#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../misc/sphere_drawing.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;

void Sphere::collide(PointMass &pm) {
    // TODO (Part 3): Handle collisions with spheres.
    if((pm.position-origin).norm() <= radius){
        //inside or on the sphere
        Vector3D direction = (pm.position-origin).unit();
        Vector3D tangent = origin + direction*radius;
        Vector3D correction_dist = (1-friction)*(tangent-pm.last_position);
        pm.position = pm.last_position + correction_dist;
    }
}

void Sphere::render(GLShader &shader) {
    // We decrease the radius here so flat triangles don't behave strangely
    // and intersect with the sphere when rendered
    m_sphere_mesh.draw_sphere(shader, origin, radius * 0.92);
}
