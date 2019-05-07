#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform vec4 u_color;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

float h(vec2 uv) {
    return texture(u_texture_2, uv)[0];
}

void main() {
    // YOUR CODE HERE
    vec3 b = cross(v_normal.xyz, v_tangent.xyz);
    mat3 TBN = mat3(v_tangent, b, v_normal);
    float width = u_texture_2_size[0];
    float height = u_texture_2_size[1];
    float kh = u_height_scaling;
    float kn = u_normal_scaling;
    float dU = (h(vec2(v_uv[0]+(1/width), v_uv[1])) - h(v_uv)) * kh * kn;
    float dV = (h(vec2(v_uv[0], v_uv[1]+(1/height))) - h(v_uv)) * kh * kn;
    vec3 nO = vec3(-dU, -dV, 1.0);
    vec3 nD = TBN*nO;
    float r = length(u_light_pos - v_position.xyz);
    vec3 Ir2 = u_light_intensity / (r*r);
    float ka = 0.1;
    float kd = 1.0;
    float ks = 0.5;
    vec3 Ia = vec3(1.0, 1.0, 1.0);
    float p = 100;
    vec3 bisector = normalize(v_position.xyz + u_light_pos);
    out_color = vec4((ka*Ia) + (kd*Ir2*max(0, dot(nD, v_position.xyz))) + (ks*Ir2*pow(max(0, dot(nD, bisector)), p)), 1);
    out_color.a = 1;
}

