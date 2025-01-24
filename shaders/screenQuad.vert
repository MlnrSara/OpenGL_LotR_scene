#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec2 fTexCoords;

uniform float offsetY;

void main() 
{
	fTexCoords = vec2(vTexCoords.x, vTexCoords.y + offsetY);
	gl_Position = vec4(vPosition, 1.0f);
}
