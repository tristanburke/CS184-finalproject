#ifndef CGL_UTIL_MESHDRAWING_H
#define CGL_UTIL_MESHDRAWING_H

#include <vector>

#include <nanogui/nanogui.h>

#include "CGL/CGL.h"
#include "OBJ_Loader.h"

using namespace nanogui;
        
class FileMesh {
public:
    // Supply the desired number of vertices
    FileMesh(objl::Loader &loader);
    
    
    /**
     * Draws a mesh with the given object loader class that is built from a file name (.obj file type)
     * current modelview/projection matrices and color/material settings.
     */
    void draw_mesh(GLShader &shader);
private:
//            objl::Loader &loader; pass in to build_Data as parameter
//            std::vector<unsigned int> Indices;
//            std::vector<double> Vertices;
    int num_indices;
    int num_vertices;
    void build_data(objl::Loader &loader);
    
    MatrixXf positions;
    MatrixXf normals;
    MatrixXf uvs;
};

#endif // CGL_UTIL_MESHDRAWING_H
