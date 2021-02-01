#version 430 core

uniform sampler2D textureSampler;
uniform sampler2D normalSampler; 
uniform vec3 spotlightDir;
uniform float spotlightCutOff;
uniform float spotlightOuterCutOff;

in vec2 interpTexCoord;
in vec3 lightDirTS;
in vec3 viewDirTS;
in vec3 spotlightDirTS;

void main()
{
	
	vec3 V = normalize(viewDirTS);
	vec3 normal = (texture2D(normalSampler, interpTexCoord).rgb * 2) - 1;
	vec3 color = texture2D(textureSampler, interpTexCoord).rgb;
	
	//Sun
	vec3 L = -normalize(lightDirTS);
	vec3 R = reflect(-normalize(L), normal);
	
	float diffuse = max(0, dot(normal, L));
	float specular_pow = 10;
	float specular = pow(max(0, dot(R, V)), specular_pow);

	vec3 lightColor = vec3(1);
	vec3 shadedColor = color * diffuse + lightColor * specular;
	
	//Flashlight
	//Odznaczyc odtąd
	// vec3 S_L = normalize(spotlightDirTS);
	// float theta = dot(S_L, normalize(-spotlightDir));
	// float epsilon   = spotlightCutOff - spotlightOuterCutOff;
	// float intensity = clamp((theta - spotlightOuterCutOff) / epsilon, 0.0, 1.0); 
	// float S_diffuse = max(0, dot(normal, S_L));
	// vec3 S_R = reflect(-S_L, normal);
	// float S_specular = pow(max(dot(V, S_R), 0.0), 4.0);
	
	// vec3 S_shadedColor = color * S_diffuse *intensity + lightColor *S_specular * intensity;
	//dotąd by mieć zepsutą latarkę U.U
	
	float ambient = 0.2;
	 gl_FragColor = vec4(mix(color, shadedColor, 1.0 - ambient), 1.0);
	 // gl_FragColor = vec4(mix(color, shadedColor, 1.0 - ambient) + mix(color, S_shadedColor, 1.0 - ambient), 1.0);
	//gl_FragColor = vec4((shadedColor + S_shadedColor), 1.0);
}
