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

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

void main() {
  // Your awesome shader here!

  float band_num = 4.0;
  float normal_range = 2.0 / band_num;


  vec4 max_color = vec4(1,0,0,1);
  vec4 min_color = vec4(0.2,0,0,1);

  float rx = (max_color[0] - min_color[0]) / band_num;
  float bx = (max_color[1] - min_color[1]) / band_num;
  float gx = (max_color[2] - min_color[2]) / band_num;

  float index = dot(vec4(u_light_pos,1), v_normal);
  for (int i = 0; i < band_num; i++) {
  	float i_float = float(i);
  	float curr_min = -1.0 + i_float * normal_range;
  	float curr_max = -1.0 + (i_float+1) * normal_range;
  	if (index >= curr_min && index <= curr_max) {
  		out_color = min_color + ((max_color - min_color) / band_num) * i_float;
  	}
  }
  if (index >= 1 - normal_range) {
  	out_color = max_color;
  }

  // Specular Highlights
  vec4 camDir = normalize(vec4(u_cam_pos,1) - v_position);
  vec4 wi = normalize(v_position  - vec4(u_light_pos,1));
  vec4 wo = 2 * dot(v_normal, wi) * v_normal - wi;
  vec4 H = normalize(wo + camDir / length(wo + camDir));
  float eta = 0.08;
  if (dot(H, v_normal) > 1 - eta) {
  	out_color = vec4(1,1,1,1);
  }

  // Edge Detection


  out_color.a = 1;


}
