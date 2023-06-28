#version 330 core

in vec2 textureCoordinate;
in vec3 Normal;
in vec3 fragmentPosition;


uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

out vec4 fragColor;

void main(){
	vec3 norm = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - fragmentPosition);

	float diff = max(dot(norm, lightDirection), 0.0);
	vec3 diffuse = diff * lightColor;

	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	vec3 result = (ambient + diffuse) * objectColor;


    fragColor = vec4(result, 1.0);
}
