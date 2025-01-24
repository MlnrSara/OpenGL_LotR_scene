#version 410 core

in vec3 fPosition;
in vec2 fTexCoords;
in vec3 fNormal;

out vec4 fColor;

// texture
uniform sampler2D diffuseTexture;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

//components
vec3 ambient;
float ambientStrength = 0.1f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(fNormal);

    //normalize light direction
    vec3 lightDirN = normalize(lightDir);

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    specular = specularStrength * specCoeff * lightColor;
}

float computeFog() { 
    vec4 fPosEye = vec4(fPosition, 1.0f);
	float fogDensity = 0.03f; 
	float fragmentDistance = length(fPosEye); 
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2)); 
	return clamp(fogFactor, 0.0f, 1.0f); 
}

void main() 
{
    computeDirLight();

    //compute final vertex color
    vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular, 1.0f);
    //vec3 color = texture(diffuseTexture, fTexCoords).rgb;
    float fogFactor = computeFog();
	vec4 fogColor = vec4(0.8f, 0.1f, 0.1f, 1.0f);
	vec4 auxColor = vec4(color, 1.0f);
	fColor = fogColor * (1-fogFactor) + auxColor * fogFactor;
    //fColor = vec4(color, 1.0f);
    //fColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}
