#ifndef CGL_UTIL_MESHDRAWING_H
#define CGL_UTIL_MESHDRAWING_H

#include <vector>

#include <nanogui/nanogui.h>

#include "CGL/CGL.h"

using namespace nanogui;

namespace CGL {
    namespace Misc {
        
        class FileMesh {
        public:
            // Supply the desired number of vertices
            FileMesh(/*somehow insert filename here*/);
            
            
            /**
             * Draws a sphere with the given position and radius in opengl, using the
             * current modelview/projection matrices and color/material settings.
             */
            void draw_sphere(GLShader &shader);
        private:
            std::vector<unsigned int> Indices;
            std::vector<double> Vertices;
            
            int s_index(int x, int y);
            
            void build_data();
            
            int num_vertices;
            int num_indices;
            
            MatrixXf positions;
            MatrixXf normals;
        };
        
        
    } // namespace Misc
} // namespace CGL

#endif // CGL_UTIL_MESHDRAWING_H
