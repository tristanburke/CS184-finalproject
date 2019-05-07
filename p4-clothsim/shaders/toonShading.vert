#version 330

layout(location = 0) in vec4 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

uniform mat3 synth_NormalMatrix;
uniform mat4 synth_ViewMatrix;
uniform mat4 synth_ProjectionMatrix;
uniform mat4 ModelMatrix;

out vec3 vPosition;
out vec3 vNormal;

void main()
{
    vPosition = (synth_ViewMatrix * ModelMatrix * VertexPosition).xyz;
    vNormal = synth_NormalMatrix * VertexNormal;
    
    gl_Position =  synth_ProjectionMatrix * vec4(vPosition, 1.0);
}
