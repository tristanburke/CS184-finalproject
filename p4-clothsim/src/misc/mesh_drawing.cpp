#include <cmath>
#include <nanogui/nanogui.h>

#include "mesh_drawing.h"

#include "../leak_fix.h"

#include "CGL/color.h"
#include "CGL/vector3D.h"

#define TCOORD_OFFSET 0
#define NORMAL_OFFSET 2
#define VERTEX_OFFSET 5
#define TANGEN_OFFSET 8
#define VERTEX_SIZE 11

using namespace nanogui;

namespace CGL {
    namespace Misc {
        
        FileMesh::FileMesh(int num_indices, int num_vertices)
        :  {
            
            Indices.resize(num_indices);
            Vertices.resize(VERTEX_SIZE * num_vertices);
            
            build_data();
        }
        
        void FileMesh::build_data() {
            
            positions = MatrixXf(4, num_indices * 3);
            normals = MatrixXf(4, num_indices * 3);
            
            for (int i = 0; i < num_indices; i += 3) {
                double *vPtr1 = &Vertices[VERTEX_SIZE * Indices[i]];
                double *vPtr2 = &Vertices[VERTEX_SIZE * Indices[i + 1]];
                double *vPtr3 = &Vertices[VERTEX_SIZE * Indices[i + 2]];
                
                Vector3D p1(vPtr1[VERTEX_OFFSET], vPtr1[VERTEX_OFFSET + 1],
                            vPtr1[VERTEX_OFFSET + 2]);
                Vector3D p2(vPtr2[VERTEX_OFFSET], vPtr2[VERTEX_OFFSET + 1],
                            vPtr2[VERTEX_OFFSET + 2]);
                Vector3D p3(vPtr3[VERTEX_OFFSET], vPtr3[VERTEX_OFFSET + 1],
                            vPtr3[VERTEX_OFFSET + 2]);
                
                Vector3D n1(vPtr1[NORMAL_OFFSET], vPtr1[NORMAL_OFFSET + 1],
                            vPtr1[NORMAL_OFFSET + 2]);
                Vector3D n2(vPtr2[NORMAL_OFFSET], vPtr2[NORMAL_OFFSET + 1],
                            vPtr2[NORMAL_OFFSET + 2]);
                Vector3D n3(vPtr3[NORMAL_OFFSET], vPtr3[NORMAL_OFFSET + 1],
                            vPtr3[NORMAL_OFFSET + 2]);
                
                positions.col(i    ) << p1.x, p1.y, p1.z, 1.0;
                positions.col(i + 1) << p2.x, p2.y, p2.z, 1.0;
                positions.col(i + 2) << p3.x, p3.y, p3.z, 1.0;
                
                normals.col(i    ) << n1.x, n1.y, n1.z, 0.0;
                normals.col(i + 1) << n2.x, n2.y, n2.z, 0.0;
                normals.col(i + 2) << n3.x, n3.y, n3.z, 0.0;
            }
        }
        
        void FileMesh::draw_mesh(GLShader &shader) {
            shader.uploadAttrib("in_position", positions);
            if (shader.attrib("in_normal", false) != -1) {
                shader.uploadAttrib("in_normal", normals);
            }
            
            shader.drawArray(GL_TRIANGLES, 0, sphere_num_indices);
#ifdef LEAK_PATCH_ON
            shader.freeAttrib("in_position");
            if (shader.attrib("in_normal", false) != -1) {
                shader.freeAttrib("in_normal");
            }
#endif
        }
        
    } // namespace Misc
} // namespace CGL
