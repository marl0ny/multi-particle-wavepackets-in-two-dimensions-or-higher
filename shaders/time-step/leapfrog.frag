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


uniform sampler2D rePsiTex;
uniform sampler2D imPsiTex;
uniform sampler2D intPotentialTex;
uniform sampler2D extPotentialTex;
uniform bool isRealStep;
uniform bool applyAbsorbingBoundaries;
uniform float hbar;
uniform vec4 massIndices;
uniform float dt;
uniform bool applyClampingToPotential;
uniform vec2 potentialClampValues;

uniform vec4 dimensions4D;
uniform ivec4 texelDimensions4D;


vec2 to2DTextureCoordinates(vec4 textureCoordinate4D) {
    float texelWidth2D = float(texelDimensions4D[0]*texelDimensions4D[1]);
    float texelHeight2D = float(texelDimensions4D[2]*texelDimensions4D[3]);
    float x = textureCoordinate4D[0]*float(texelDimensions4D[0]);
    float y = textureCoordinate4D[1]*float(texelDimensions4D[1]);
    float z = textureCoordinate4D[2]*float(texelDimensions4D[2]);
    float w = textureCoordinate4D[3]*float(texelDimensions4D[3]);
    return vec2((x + floor(y)*float(texelDimensions4D[0]))/texelWidth2D,
                (z + floor(w)*float(texelDimensions4D[2]))/texelHeight2D);
}

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

vec4 texSample4D(sampler2D tex, vec4 texCoord4D, vec4 offset) {
    vec4 coord4D = texCoord4D + offset;
    coord4D.x = (coord4D.x < 0.0)? (1.0 + coord4D.x): coord4D.x;
    coord4D.y = (coord4D.y < 0.0)? (1.0 + coord4D.y): coord4D.y;
    coord4D.z = (coord4D.z < 0.0)? (1.0 + coord4D.z): coord4D.z;
    coord4D.w = (coord4D.w < 0.0)? (1.0 + coord4D.w): coord4D.w;
    coord4D.x = (coord4D.x >= 1.0)? (coord4D.x - 1.0): coord4D.x;
    coord4D.y = (coord4D.y >= 1.0)? (coord4D.y - 1.0): coord4D.y;
    coord4D.z = (coord4D.z >= 1.0)? (coord4D.z - 1.0): coord4D.z;
    coord4D.w = (coord4D.w >= 1.0)? (coord4D.w - 1.0): coord4D.w;
    vec2 uv = to2DTextureCoordinates(texCoord4D + offset);
    return texture2D(tex, uv);
}

float laplacian2ndOr3Pt1D(sampler2D psiTex, vec4 texCoord4D,
                          int dimensionIndex, int channelIndex,
                          float psiCenter) {
    float domainLength = dimensions4D[dimensionIndex];
    float texelLength = float(texelDimensions4D[dimensionIndex]);
    float texelStepSize = 1.0/texelLength;
    float spatialStepSize = domainLength/texelLength;
    vec4 offset = vec4(0.0);
    offset[dimensionIndex] = texelStepSize;
    float neighbours = texSample4D(psiTex, texCoord4D, -offset)[channelIndex];
    neighbours += texSample4D(psiTex, texCoord4D, offset)[channelIndex];
    return (neighbours - 2.0*psiCenter)/spatialStepSize;
}

float laplacian4thOr5Pt1D(sampler2D psiTex, vec4 texCoord4D,
                          int dimensionIndex, int channelIndex,
                          float psiCenter) {
    float domainLength = dimensions4D[dimensionIndex];
    float texelLength = float(texelDimensions4D[dimensionIndex]);
    float texelStepSize = 1.0/texelLength;
    float spatialStepSize = domainLength/texelLength;
    vec4 offset = vec4(0.0);
    offset[dimensionIndex] = texelStepSize;
    float neighbours 
        = 4.0*texSample4D(psiTex, texCoord4D, -offset)[channelIndex]/3.0;
    neighbours 
        += 4.0*texSample4D(psiTex, texCoord4D, offset)[channelIndex]/3.0;
    neighbours
        += -texSample4D(psiTex, texCoord4D, -2.0*offset)[channelIndex]/12.0;
    neighbours
        += -texSample4D(psiTex, texCoord4D, 2.0*offset)[channelIndex]/12.0;
    return (neighbours - (5.0/2.0)*psiCenter)/spatialStepSize;
}

float applyAbsorbing(float psi) {
    if (applyAbsorbingBoundaries) {
        vec4 coord = to4DTextureCoordinates(UV);
        float x = coord[0], y = coord[1], z = coord[2], w = coord[3];
        float dampFactor = 0.0;
        float s = 0.02;
        float a = 0.8;
        dampFactor += a*exp(-0.5*x*x/(s*s));
        dampFactor += a*exp(-0.5*(x-1.0)*(x-1.0)/(s*s));
        dampFactor += a*exp(-0.5*y*y/(s*s));
        dampFactor += a*exp(-0.5*(y-1.0)*(y-1.0)/(s*s));
        dampFactor += a*exp(-0.5*z*z/(s*s));
        dampFactor += a*exp(-0.5*(z-1.0)*(z-1.0)/(s*s));
        dampFactor += a*exp(-0.5*w*w/(s*s));
        dampFactor += a*exp(-0.5*(w-1.0)*(w-1.0)/(s*s));
        dampFactor *= dt;
        return psi*(1.0 - dampFactor);
    }
    return psi;
}

float zeroPotentialIfInsideAbsorbingBoundaries(float potential) {
    vec4 texCoord4D = to4DTextureCoordinates(UV);
    float s = 0.02;
    if (applyAbsorbingBoundaries &&
        (texCoord4D[0] <= 1.0*s || texCoord4D[0] > (1.0 - 1.0*s) || 
         texCoord4D[1] <= 1.0*s || texCoord4D[1] > (1.0 - 1.0*s) ||
         texCoord4D[2] <= 1.0*s || texCoord4D[2] > (1.0 - 1.0*s) ||
         texCoord4D[3] <= 1.0*s || texCoord4D[3] > (1.0 - 1.0*s)))
        potential = 0.0;
    return potential;
}

float getExternalPotential(sampler2D extPotentialTex, vec4 texCoord4D) {
    return texture2D(extPotentialTex, texCoord4D.xz)[0]
        + texture2D(extPotentialTex, texCoord4D.yw)[0];
}

float clampPotential(float potential, float minVal, float maxVal) {
    if (maxVal == minVal)
        return minVal;
    if (potential > maxVal)
        return maxVal;
    else if (potential <= minVal)
        return minVal;
    return potential;
}

float hamiltonian(sampler2D psiTex) {
    vec4 texCoord4D = to4DTextureCoordinates(UV);
    float potential = zeroPotentialIfInsideAbsorbingBoundaries(
        texture2D(intPotentialTex, UV)[0] 
        + getExternalPotential(extPotentialTex, texCoord4D));
    if (applyClampingToPotential)
        potential = clampPotential(
            potential, potentialClampValues[0], potentialClampValues[1]);
    float psiCenter = texSample4D(psiTex, texCoord4D, vec4(0.0))[0];
    float 
    kinetic = (-hbar*hbar/(2.0*massIndices[0]))
        * laplacian4thOr5Pt1D(psiTex, texCoord4D, 0, 0, psiCenter);
    kinetic += (-hbar*hbar/(2.0*massIndices[1]))
        * laplacian4thOr5Pt1D(psiTex, texCoord4D, 1, 0, psiCenter);
    kinetic += (-hbar*hbar/(2.0*massIndices[2]))
        * laplacian4thOr5Pt1D(psiTex, texCoord4D, 2, 0, psiCenter);
    kinetic += (-hbar*hbar/(2.0*massIndices[3]))
        * laplacian4thOr5Pt1D(psiTex, texCoord4D, 3, 0, psiCenter);
    return kinetic + potential*psiCenter;
}

float getNextRealPsi() {
    float rePsiPrev = texture2D(rePsiTex, UV)[0];
    return applyAbsorbing(rePsiPrev) + dt*hamiltonian(imPsiTex);

}

float getNextImagPsi() {
    float imPsiPrev = texture2D(imPsiTex, UV)[0];
    return applyAbsorbing(imPsiPrev) - dt*hamiltonian(rePsiTex);
}

void main() {
    fragColor = (isRealStep)?
        vec4(getNextRealPsi()): vec4(getNextImagPsi());
}