#version 330

uniform vec4 u_color;
uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

in vec4 v_position;
in vec4 v_normal;
in vec2 v_uv;

out vec4 out_color;

void main() {
    // YOUR CODE HERE
    
    // (Placeholder code. You will want to replace it.)
    float r = length(u_light_pos - v_position.xyz);
    vec3 Ir2 = u_light_intensity / (r*r);
    float ka = 0.1;
    float kd = 1.0;
    float ks = 0.5;
    vec3 Ia = vec3(1.0, 1.0, 1.0);
    float p = 100;
    vec3 bisector = normalize(v_position.xyz + u_light_pos);
    out_color = vec4((ka*Ia) + (kd*Ir2*max(0, dot(v_normal, v_position))) + (ks*Ir2*pow(max(0, dot(v_normal.xyz, bisector)), p)), 1);
    out_color.a = 1;
}

