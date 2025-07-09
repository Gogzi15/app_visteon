#version 300 es

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec2 vTexCoords;
out vec3 vNormal;

uniform float time;

void main()
{
	vTexCoords = texCoord;
	vNormal = normal;
	gl_Position = vec4(position, 1.0);
}
