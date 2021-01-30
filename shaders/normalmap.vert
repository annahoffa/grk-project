#version 430 core

uniform vec3 lightDir;
uniform vec3 cameraPos;
uniform vec3 vertPos;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelMatrix;

out vec3 interpNormal;
out vec2 interpTexCoord;
out vec3 fragPos;

out vec3 lightDirTS;
out vec3 viewDirTS;
out mat3 TBN;

void main()
{
	vec3 normal = vec3(modelMatrix * vec4(vertexNormal, 0.0));
	vec3 tangent = vec3(modelMatrix * vec4(vertexTangent, 0.0));
	vec3 bitangent = vec3(modelMatrix * vec4(vertexBitangent, 0.0));
	TBN = transpose(mat3(tangent, bitangent, normal));
	vec3 viewDir = normalize(cameraPos - vertPos);
	lightDirTS = TBN * lightDir;
	viewDirTS = TBN * viewDir;
	
	gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
	fragPos = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;

	interpNormal = (modelMatrix * vec4(vertexNormal, 0.0)).xyz;
	interpTexCoord = vertexTexCoord;
}
