#version 300 es

precision highp float;
//precision highp sampler2D;

out vec4 FragColor;

// For the moment we don't support samplers, audio and mouse
uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform float     iTimeDelta;            // render time (in seconds)
uniform float     iFrameRate;            // shader frame rate
uniform int       iFrame;                // shader playback frame
//uniform float     iChannelTime[4];       // channel playback time (in seconds)
//uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)
//uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
//uniform sampler2D iChannel0;             // input channel. XX = 2D/Cube
//uniform sampler2D iChannel1;             // input channel. XX = 2D/Cube
//uniform sampler2D iChannel2;             // input channel. XX = 2D/Cube
//uniform sampler2D iChannel3;             // input channel. XX = 2D/Cube
uniform vec4      iDate;                 // (year, month, day, time in seconds)
//uniform float     iSampleRate;           // sound sample rate (i.e., 44100)

void mainImage( out vec4 fragColor, in vec2 fragCoord );

void main()
{
	mainImage(FragColor, gl_FragCoord.xy);
}

// Ideas to test:
// https://www.shadertoy.com/new
// https://www.shadertoy.com/view/4t3SzN
// https://www.shadertoy.com/view/Xtf3zn

// ================== Put your shadertoy code here ==================

// https://www.shadertoy.com/new
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;

    // Time varying pixel color
    vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));

    // Output to screen
    fragColor = vec4(col,1.0);
}

// ================================================================

