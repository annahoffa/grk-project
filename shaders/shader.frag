#version 430 core

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform struct Light{
    vec3  position;
    vec3  direction;
    float cutOff;
} light[1];    

in vec3 interpNormal;
in vec3 vecPosition;

out vec4 FragColor;

vec3 spotLight(Light light){
	float theta = dot(light.direction, normalize(-light.direction));
	if(theta > light.cutOff) {       
		vec3 normal = normalize(interpNormal);
		light.direction = normalize(vecPosition - light.position);
		float diffuse = max(dot(normal, -light.direction), 0.0);
		vec3 Color = vec3(objectColor * diffuse);
		return Color;
	}
}

vec3 sun(){
	vec3 normal = normalize(interpNormal);
	vec3 V = normalize(cameraPos - vecPosition);
	vec3 lightDir = normalize(vecPosition - lightPos);
	
	float diffuse = max(dot(normal, -lightDir), 0.0);
	vec3 R = reflect(lightDir, normal);
	float specular = pow(max(dot(V, R), 0.0), 4.0);
	
	vec3 Color = vec3(objectColor * diffuse + vec3(1.0) * specular);
	return Color;
}

void main()
{	
	vec3 outy = vec3(0.0);
	outy = outy +  sun() + spotLight(light[0]);
	vec4 FragColor = vec4(outy, 1.0);
}
