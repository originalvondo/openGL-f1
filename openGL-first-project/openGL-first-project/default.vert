#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoord;
layout (location = 2) in vec3 aNormal;

out vec2 textureCoordinate;
out vec3 Normal;
out vec3 fragmentPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = projection * view * model * vec4(position, 1.0);
	textureCoordinate = textureCoord;
	Normal = aNormal;
	fragmentPosition = vec3(model * vec4(position, 1.0));
}