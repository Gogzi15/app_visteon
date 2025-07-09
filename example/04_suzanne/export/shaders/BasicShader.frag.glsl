#version 300 es

out highp vec4 FragColor;
in highp vec2 vTexCoords;
in highp vec3 vNormal;



void main()
{
	FragColor = vec4(vNormal, 1.0);
}
