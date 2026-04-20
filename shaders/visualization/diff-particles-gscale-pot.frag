/* DIFFerentiate PARTICLES with separate colors,
use GraySCALE for the POTential.
*/
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

uniform sampler2D prob1Tex;
uniform sampler2D prob2Tex;
uniform sampler2D potTex;
uniform sampler2D prevExternalVisTex;
uniform vec3 color1;
uniform vec3 color2;
uniform float waveFunctionBrightness1;
uniform float waveFunctionBrightness2;
uniform float potentialBrightness;

void main() {
    float p1 = waveFunctionBrightness2*texture2D(prob1Tex, UV)[0];
    float p2 = waveFunctionBrightness1*texture2D(prob2Tex, UV)[0];
    float potentialVal = potentialBrightness*texture2D(potTex, UV)[0];
    vec3 c1 = p1*color1;
    vec3 c2 = p2*color2;
    vec4 previousExternalVis = texture2D(prevExternalVisTex, UV);
    fragColor = vec4(potentialVal + c1 + c2 + previousExternalVis.rgb, 1.0);
}