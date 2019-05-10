#version 330

// (Every uniform is available here.)

uniform mat4 u_view_projection;
uniform mat4 u_model;

uniform float u_normal_scaling;
uniform float u_height_scaling;

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

// Feel free to add your own textures. If you need more than 4,
// you will need to modify the skeleton.
uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;
uniform sampler2D u_texture_3;
uniform sampler2D u_texture_4;

// Environment map! Take a look at GLSL documentation to see how to
// sample from this.
uniform samplerCube u_texture_cubemap;

// ADDED IN
uniform float u_eta;
uniform int u_band_count;
uniform float u_edge_width;
uniform int u_highlights;
uniform int u_edges;
uniform vec4 u_min_color;
uniform vec4 u_max_color;
uniform vec4 u_outline_color;
uniform float u_spec_intensity;
uniform float u_shade;
//uniform vec4 u_min_color;
//uniform float u_spec;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

void main() {
    // Your awesome shader here!
    float band_num = u_band_count;
    if (band_num < 1) {
        band_num = 1;
    }
    float normal_range = 2.0 / band_num;
    
    float rx = (u_max_color[0] - u_min_color[0]) / band_num;
    float bx = (u_max_color[1] - u_min_color[1]) / band_num;
    float gx = (u_max_color[2] - u_min_color[2]) / band_num;
    
    //float start = 0.5;
    
    out_color = texture(u_texture_1, v_uv) * u_shade;
    
    float index = dot(vec4(u_light_pos,1), v_normal);
    //index = atan(index);
    //index = (1 / (1 + exp(-index)));
    
    //float shade = (1 - start) * 2 / band_num;
    float shade = 0.0;
    
    for (int i = 0; i < band_num; i++) {
        float i_float = float(i);
        float curr_min = -1.0 + i_float * normal_range;
        float curr_max = curr_min + normal_range;
        if (index >= curr_min && index <= curr_max) {
            out_color = texture(u_texture_1, v_uv) * (u_shade + shade);

            //out_color = u_min_color + ((u_max_color - u_min_color) / band_num) * i_float;
        }
        shade += (1 - u_shade) * 2 / band_num;
    }
    if (index >= 1 - normal_range) {
        out_color = texture(u_texture_1, v_uv) * (2.0 - u_shade);
    }
    
    // Specular Highlights
    vec4 camDir = normalize(vec4(u_cam_pos,1) - v_position);
    vec4 wi = normalize(vec4(u_light_pos,1) - v_position);
    vec4 H = normalize(wi + camDir / length(wi + camDir));
    if (u_highlights == 1) {
        if (dot(H, v_normal) > 1 - u_eta) {
            out_color += u_spec_intensity * vec4(1, 1, 1, 1);
            //            out_color = vec4(1,1,1,1);
        }
    }
    
    // Edge Detection
    if (u_edges == 1) {
        //vec4 edge_color = vec4(0,0,0,1);
        float edge_width = u_edge_width;
        //float edge_width = 0.4;
        if (edge_width <= 0.0) {
            edge_width = 0.001;
        }
        float edge = dot(v_normal, camDir);
        edge = clamp(edge, 0, 1);
        if(edge < edge_width) {
            out_color = u_outline_color;
        }
    }
    out_color.a = 1;
}
