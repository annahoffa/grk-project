#version 430 core

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 spotlightPos;
uniform vec3 spotlightDir;
uniform float spotlightCutOff;

in vec3 interpNormal;
in vec3 vecPosition;

vec3 spotLight(){
	float theta = dot(spotlightDir, normalize(-spotlightDir));
	if(theta > spotlightCutOff) {       
		vec3 normal = normalize(interpNormal);
		vec3 Dir = normalize(vecPosition - spotlightPos);
		float diffuse = max(dot(normal, -Dir), 0.0);
		vec3 Color = vec3(objectColor * diffuse);
		return(Color);
	}
}

// vec3 sun(){
	// vec3 normal = normalize(interpNormal);
	// vec3 V = normalize(cameraPos - vecPosition);
	// vec3 lightDir = normalize(vecPosition - lightPos);
	
	// float diffuse = max(dot(normal, -lightDir), 0.0);
	// vec3 R = reflect(lightDir, normal);
	// float specular = pow(max(dot(V, R), 0.0), 4.0);
	
	// vec3 Color= vec3(objectColor * diffuse + vec3(1.0) * specular);
	// return(Color);
// }

void main()
{	
	gl_FragColor = vec4(spotLight(), 1.0);
}
