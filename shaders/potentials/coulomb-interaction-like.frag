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

uniform ivec2 indices1;
uniform ivec2 indices2;
uniform ivec4 texelDimensions4D;
uniform vec4 dimensions4D;
uniform float largestAllowedPotentialValue;
uniform float interactionStrength;

#define PI 3.141592653589793

vec4 to4DTextureCoordinates(vec2 textureCoordinate2D) {
    float texelWidth2D = float(texelDimensions4D[0]*texelDimensions4D[1]);
    float texelHeight2D = float(texelDimensions4D[2]*texelDimensions4D[3]);
    vec2 texelPosition2D = vec2(textureCoordinate2D[0]*texelWidth2D,
                                textureCoordinate2D[1]*texelHeight2D);
    float x = mod(texelPosition2D[0], float(texelDimensions4D[0]));
    float y = floor(texelPosition2D[0] / float(texelDimensions4D[0])) + 0.5;
    float z = mod(texelPosition2D[1], float(texelDimensions4D[2]));
    float w = floor(texelPosition2D[1] / float(texelDimensions4D[2])) + 0.5;
    return vec4(
        x/float(texelDimensions4D[0]), y/float(texelDimensions4D[1]),
        z/float(texelDimensions4D[2]), w/float(texelDimensions4D[3]));
}

float getExternalPotential(vec2 r1, vec2 r2) {
    return (r1.x*r1.x + r1.y*r1.y) + (r2.x*r2.x + r2.y*r2.y);
}

void main() {
    vec4 normalizedCoord4D = to4DTextureCoordinates(UV);
    vec4 r1R2 = normalizedCoord4D*dimensions4D;
    vec2 r1 = vec2(r1R2[indices1[0]], r1R2[indices1[1]]);
    vec2 r2 = vec2(r1R2[indices2[0]], r1R2[indices2[1]]);
    vec2 r = r1 - r2;
    float absR = length(r);
    float extPotential = getExternalPotential(
        vec2(normalizedCoord4D[indices1[0]],
             normalizedCoord4D[indices1[1]]) - vec2(0.5, 0.5),
        vec2(normalizedCoord4D[indices2[0]],
             normalizedCoord4D[indices2[1]]) - vec2(0.5, 0.5));
    float potential = (absR > 0.0)?
        interactionStrength/absR + extPotential: 
        sign(interactionStrength)*largestAllowedPotentialValue;
    if (interactionStrength == 0.0)
        potential = extPotential;
    // potential = min(largestAllowedPotentialValue, potential);
    fragColor = vec4(potential, 0.0, potential, 0.0);
    
}