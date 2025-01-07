#version 410 core

in vec2 fTexCoords;

out vec4 fColor;

uniform sampler2D depthMap;

void main() 
{    
    vec4 colorFromTexture = texture(depthMap, fTexCoords);
    if(colorFromTexture.a <0.1f)
        discard;
    fColor = colorFromTexture;
    //fColor = vec4(fTexCoords, 0.0f, 1.0f);
}
