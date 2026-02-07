#if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
#define texture2D texture
#else
#define texture texture2D
#endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif
 
#if __VERSION__ <= 120
varying vec2 UV;
#define fragColor gl_FragColor
#else
in vec2 UV;
out vec4 fragColor;
#endif

#define complex vec2

uniform sampler2D reTex;
uniform sampler2D imTex1;
uniform sampler2D imTex2;

void main() {
    float re = texture2D(reTex, UV)[0];
    float im1 = texture2D(imTex1, UV)[0];
    float im2 = texture2D(imTex2, UV)[0];
    fragColor = vec4(re*re + im1*im2);
}
