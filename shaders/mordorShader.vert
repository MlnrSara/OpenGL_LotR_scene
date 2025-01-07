#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fNormal;
out vec4 fPosEye;
out vec2 fTexCoords;
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;
uniform mat4 lightSpaceTrMatrix[4];
uniform float cascadeSplits[4];

void main() 
{
	//compute eye space coordinates
	fPosEye = view * model * vec4(vPosition, 1.0f);
	fNormal = normalize(normalMatrix * vNormal);
	fTexCoords = vTexCoords;
	for (int i = 0; i < 4; ++i) {
        if (fPosEye.z < cascadeSplits[i]) {
            fragPosLightSpace = lightSpaceTrMatrix[i] * model * vec4(vPosition, 1.0f);
            break;
        }
    }
	
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
}