#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

//for the positional lights
float linear = 0.09f;
float quadratic = 0.032f;
float constant = 1.0f;

uniform vec3 eyeLightPos;
uniform vec3 positionalLightColor; 

uniform mat4 view;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(view*vec4(lightDir,0.0f)).xyz;
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;

	vec3 lightPos = (view * vec4(eyeLightPos, 1.0f)).xyz;
	normalEye = normalize(fNormal);
	viewDirN = normalize(cameraPosEye - fPosEye.xyz);
	lightDirN = normalize(lightPos - fPosEye.xyz);
	float dist = length(lightPos - fPosEye.xyz);
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
	vec3 halfVector = normalize(lightDirN + viewDirN);
	ambient += att * ambientStrength * positionalLightColor;
	diffuse += att * max(dot(normalEye, lightDirN), 0.0f) * positionalLightColor;
	specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular += att * specularStrength * specCoeff * positionalLightColor;
}

float computeShadow(){
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;

	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;

	float bias = 0.005f;
	float shadow = currentDepth-bias > closestDepth ? 1.0 : 0.0;
	if (normalizedCoords.z > 1.0f) return 0.0f;

	return shadow;
}

float computeFog() { 
	float fogDensity = 0.03f; 
	float fragmentDistance = length(fPosEye); 
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2)); 
	return clamp(fogFactor, 0.0f, 1.0f); 
}

void main() 
{
	computeLightComponents();
	
	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	float shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);

	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.8f, 0.1f, 0.1f, 1.0f);
	vec4 auxColor = vec4(color, 1.0f);
	fColor = fogColor * (1-fogFactor) + auxColor * fogFactor;
    
    //fColor = vec4(color, 1.0f);
}
