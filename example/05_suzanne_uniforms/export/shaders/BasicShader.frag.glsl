#version 300 es

out highp vec4 FragColor;
in highp vec2 vTexCoords;
in highp vec3 vNormal;

uniform highp float time;
uniform highp vec4 baseColor;

void main()
{
	highp vec3 color = mix(baseColor.rgb, vNormal, abs(sin(time)));
	FragColor = vec4(color, baseColor.a);
}
