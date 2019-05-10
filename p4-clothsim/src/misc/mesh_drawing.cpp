#include <cmath>
#include <nanogui/nanogui.h>

#include "mesh_drawing.h"

#include "../leak_fix.h"

#include "CGL/color.h"
#include "CGL/vector3D.h"

#include "OBJ_Loader.h"

#define TCOORD_OFFSET 0
#define NORMAL_OFFSET 2
#define VERTEX_OFFSET 5
#define TANGEN_OFFSET 8
#define VERTEX_SIZE 11

using namespace nanogui;

FileMesh::FileMesh(objl::Loader &loader)
{
//            Indices.resize(loader->LoadedMeshes[0].Indices.size());
//            Vertices.resize(loader->LoadedMeshes[0].Vertices.size());
    num_indices = loader.LoadedMeshes[0].Indices.size();
    num_vertices = loader.LoadedMeshes[0].Vertices.size();
    build_data(loader);
}

void FileMesh::build_data(objl::Loader &loader) {
    
    
    std::cout << "NUM INDICES " << num_indices << ", NUM VERT " << num_vertices << std::endl;
    
    positions = MatrixXf(4, num_indices * 3);
    normals = MatrixXf(4, num_indices * 3);
    uvs = MatrixXf(2, num_indices * 3);
    
    for (int i = 0; i < num_indices; i += 3) {
        objl::Vertex *vPtr1 = &loader.LoadedMeshes[0].Vertices[loader.LoadedMeshes[0].Indices[i]];
        objl::Vertex *vPtr2 = &loader.LoadedMeshes[0].Vertices[loader.LoadedMeshes[0].Indices[i+1]];
        objl::Vertex *vPtr3 = &loader.LoadedMeshes[0].Vertices[loader.LoadedMeshes[0].Indices[i+2]];
        
        CGL::Vector3D p1(vPtr1->Position.X, vPtr1->Position.Y,
                    vPtr1->Position.Z);
        CGL::Vector3D p2(vPtr2->Position.X, vPtr2->Position.Y,
                    vPtr2->Position.Z);
        CGL::Vector3D p3(vPtr3->Position.X, vPtr3->Position.Y,
                    vPtr3->Position.Z);

        CGL::Vector3D n1(vPtr1->Normal.X, vPtr1->Normal.Y,
                    vPtr1->Normal.Z);
        CGL::Vector3D n2(vPtr2->Normal.X, vPtr2->Normal.Y,
                    vPtr2->Normal.Z);
        CGL::Vector3D n3(vPtr3->Normal.X, vPtr3->Normal.Y,
                    vPtr3->Normal.Z);
        
        CGL::Vector3D uv1(vPtr1->TextureCoordinate.X, vPtr1->TextureCoordinate.Y, 0);
        CGL::Vector3D uv2(vPtr2->TextureCoordinate.X, vPtr2->TextureCoordinate.Y, 0);
        CGL::Vector3D uv3(vPtr3->TextureCoordinate.X, vPtr3->TextureCoordinate.Y, 0);
        
        positions.col(i    ) << p1.x, p1.y, p1.z, 1.0;
        positions.col(i + 1) << p2.x, p2.y, p2.z, 1.0;
        positions.col(i + 2) << p3.x, p3.y, p3.z, 1.0;

        normals.col(i    ) << n1.x, n1.y, n1.z, 0.0;
        normals.col(i + 1) << n2.x, n2.y, n2.z, 0.0;
        normals.col(i + 2) << n3.x, n3.y, n3.z, 0.0;
        
        uvs.col(i    ) << uv1.x, uv1.y;
        uvs.col(i + 1) << uv2.x, uv2.y;
        uvs.col(i + 2) << uv3.x, uv3.y;
    }
}

void FileMesh::draw_mesh(GLShader &shader) {
    shader.uploadAttrib("in_position", positions);
    if (shader.attrib("in_normal", false) != -1) {
        shader.uploadAttrib("in_normal", normals);
    }
    if (shader.attrib("in_uv", false) != -1) {
        shader.uploadAttrib("in_uv", uvs);
    }
    
    shader.drawArray(GL_TRIANGLES, 0, num_indices);
#ifdef LEAK_PATCH_ON
    shader.freeAttrib("in_position");
    if (shader.attrib("in_normal", false) != -1) {
        shader.freeAttrib("in_normal");
    }
    if (shader.attrib("in_uv", false) != -1) {
        shader.freeAttrib("in_uv");
    }
#endif
}

