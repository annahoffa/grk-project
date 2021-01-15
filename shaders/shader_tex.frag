#version 430 core

uniform sampler2D colorTexture;
uniform vec3 lightPos;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 vecPosition;
in vec2 vecTexCoord;

void main()
{	
	vec4 textureColor = texture2D(colorTexture, vecTexCoord);
	vec3 objectColor = textureColor.xyz;
	vec3 normal = normalize(interpNormal);
	vec3 V = normalize(cameraPos - vecPosition);
	vec3 lightDir = normalize(vecPosition - lightPos);
	
	float diffuse = max(dot(normal, -lightDir), 0.0);
	vec3 R = reflect(lightDir, normal);
	float specular = pow(max(dot(V, R), 0.0), 4.0);
	
	// yellow-ish glow from sun:
	gl_FragColor = vec4(objectColor * diffuse + vec3(1.0, 0.8, 0.6) * specular, 1.0);
}
