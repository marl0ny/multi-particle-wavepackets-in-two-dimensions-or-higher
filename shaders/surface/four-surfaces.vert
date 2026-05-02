 
#if __VERSION__ <= 120
attribute vec4 position;
varying vec4 HEIGHTS;
varying vec2 UV;
varying float SURFACE_IND;
varying vec3 FINAL_VERTEX_POSITION;
varying vec3 NORMAL;
#else
in vec4 position;
out vec4 HEIGHTS;
out vec2 UV;
out float SURFACE_IND;
out vec3 FINAL_VERTEX_POSITION;
out vec3 NORMAL;
#endif

#if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
#define texture2D texture
#else
#define texture texture2D
#endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif
    

#define quaternion vec4

#define complex vec2

uniform sampler2D heightTex1;
uniform int heightDataType1;
uniform float heightScale1;
uniform float heightOffset1;

uniform sampler2D heightTex2;
uniform int heightDataType2;
uniform float heightScale2;
uniform float heightOffset2;

uniform sampler2D heightTex3;
uniform int heightDataType3;
uniform float heightScale3;
uniform float heightOffset3;

uniform sampler2D heightTex4;
uniform int heightDataType4;
uniform float heightScale4;
uniform float heightOffset4;

uniform float scale;
uniform quaternion rotation;
uniform ivec2 screenDimensions;
uniform vec3 translate;
uniform ivec2 dimensions2D;
const int REAL_DATA_TYPE = 0;
const int COMPLEX_DATA_TYPE = 1;
const int COMPLEX_ABS_DATA_TYPE = 2;

uniform int ordering;
const int Y_ASCENDING = 1;
const int Y_DESCENDING = -1;
const int X_ASCENDING = 2;
const int X_DESCENDING = -2;

quaternion mul(quaternion q1, quaternion q2) {
    quaternion q3;
    q3.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
    q3.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y; 
    q3.y = q1.w*q2.y + q1.y*q2.w + q1.z*q2.x - q1.x*q2.z; 
    q3.z = q1.w*q2.z + q1.z*q2.w + q1.x*q2.y - q1.y*q2.x;
    return q3; 
}

quaternion conj(quaternion q) {
    return quaternion(-q.x, -q.y, -q.z, q.w);
}

quaternion rotate(quaternion x, quaternion r) {
    return quaternion(mul(conj(r), mul(x, r)).xyz, 1.0);
}

vec4 project(vec4 x) {
    vec4 y;
    y[0] = x[0]*4.0/(x[2] + 4.0);
    y[1] = float(screenDimensions[0])/float(screenDimensions[1])
            *x[1]*4.0/(x[2] + 4.0);
    y[2] = x[2]/4.0;
    y[3] = 1.0;
    return y;
}

vec3 getNormal(vec2 xy, sampler2D heightTex, float heightScale) {
    float x = xy[0], y = xy[1];
    float dx = 1.0/float(dimensions2D[0]);
    float dy = 1.0/float(dimensions2D[1]);
    float height = heightScale*texture2D(
        heightTex, xy)[0];
    float heightR = heightScale*texture2D(
        heightTex, xy + vec2(dx, 0.0))[0];
    float heightU = heightScale*texture2D(
        heightTex, xy + vec2(0.0, dy))[0];
    vec3 vC = vec3(x, y, height);
    vec3 vR = vec3(x + dx, y, heightR);
    vec3 vU = vec3(x, y + dy, heightU);
    vec3 eU = vU - vC;
    vec3 eR = vR - vC;
    /* Note that since the z-axis points away from the screen,
    and that the surface height map is inverted in the other direction,
    these normal vectors point into the surface instead of away.
    */
    return normalize(mul(quaternion(eR, 0.0), quaternion(eU, 0.0)).xyz);
}

float getHeight(sampler2D heightTex, int heightDataType) {
    float height = texture2D(heightTex, UV)[0];
    if (heightDataType == COMPLEX_DATA_TYPE) {
        complex z = texture2D(heightTex, UV).xy;
        height = z.x*z.x + z.y*z.y;
    } else if (heightDataType == COMPLEX_ABS_DATA_TYPE) {
        height = texture2D(heightTex, UV).z;
        #if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
        if (isnan(height))
            height = 0.0;
        #endif
    }
    return height;
}

void swap(inout float a, inout float b) {
    float tmp = a;
    a = b;
    b = tmp;
}

float compareVal(vec4 val) {
    return val[0];
}

void sort2(inout vec4 high, inout vec4 low, vec4 in1, vec4 in2) {
    if (compareVal(in1) > compareVal(in2)) {
        high = in1;
        low = in2;
        return;
    }
    high = in2;
    low = in1;
}

void bitonicSort4(
    inout vec4 res0, inout vec4 res1, inout vec4 res2, inout vec4 res3,
    vec4 in0, vec4 in1, vec4 in2, vec4 in3, bool sortHigh2Low
) {
    vec4 h1, low1, h2, low2;
    sort2(h1, low1, in0, in2);
    sort2(h2, low2, in1, in3);
    if (!sortHigh2Low) {
        sort2(res1, res0, low1, low2);
        sort2(res3, res2, h1, h2);
    } else {
        sort2(res0, res1, h1, h2);
        sort2(res2, res3, low1, low2);
    }
}

vec4 getPosition(float heightScale, float heightMapVal) {
    vec4 r = scale*(
        vec4(UV - vec2(0.5, 0.5), 0.0, 0.0)
        + vec4(0.0, 0.0, -heightScale*heightMapVal, 0.0));
    return rotate(r, rotation) + vec4(translate, 0.0);
}

void main() {
    int surfaceInd = int(position[0]);
    UV[0] = position[1];
    UV[1] = position[2];
    if (ordering == Y_DESCENDING) {
        UV[1] = 1.0 - position[2];
    } else if (ordering == X_ASCENDING) {
        UV[0] = position[2];
        UV[1] = position[1];
    } else if (ordering == X_DESCENDING) {
        UV[0] = 1.0 - position[2];
        UV[1] = position[1];
    }
    float height1 = getHeight(heightTex1, heightDataType1) + heightOffset1;
    float height2 = getHeight(heightTex2, heightDataType2) + heightOffset2;
    float height3 = getHeight(heightTex3, heightDataType3) + heightOffset3;
    float height4 = getHeight(heightTex4, heightDataType4) + heightOffset4;
    vec3 normal1 = getNormal(UV, heightTex1, heightScale1);
    vec3 normal2 = getNormal(UV, heightTex2, heightScale2);
    vec3 normal3 = getNormal(UV, heightTex3, heightScale3);
    vec3 normal4 = getNormal(UV, heightTex4, heightScale4);
    vec4 heightStatInit1 = vec4(height1, heightScale1, 0.0, 0.0);
    vec4 heightStatInit2 = vec4(height2, heightScale2, 1.0, 0.0);
    vec4 heightStatInit3 = vec4(height3, heightScale3, 2.0, 0.0);
    vec4 heightStatInit4 = vec4(height4, heightScale4, 3.0, 0.0);
    vec4 heightStat0, heightStat1, heightStat2, heightStat3;
    bitonicSort4(
        heightStat0, heightStat1, heightStat2, heightStat3,
        heightStatInit1, heightStatInit2, heightStatInit3, heightStatInit4,
        false);
    vec4 r;
    if (surfaceInd == 0) {
        SURFACE_IND = heightStat0[2];
        r = getPosition(heightStat0[1], heightStat0[0]);
    } else if (surfaceInd == 1) {
        SURFACE_IND = heightStat1[2];
        r = getPosition(heightStat1[1], heightStat1[0]);
    } else if (surfaceInd == 2) {
        SURFACE_IND = heightStat2[2];
        r = getPosition(heightStat2[1], heightStat2[0]);
    } else if (surfaceInd == 3) {
        SURFACE_IND = heightStat3[2];
        r = getPosition(heightStat3[1], heightStat3[0]);
    }
    HEIGHTS = vec4(
        heightStat0[0], heightStat1[0], heightStat2[0], heightStat3[0]);
    FINAL_VERTEX_POSITION = r.xyz;
    gl_Position = project(r);
    heightStatInit1 = vec4(height1, normal1);
    heightStatInit2 = vec4(height2, normal2);
    heightStatInit3 = vec4(height3, normal3);
    heightStatInit4 = vec4(height4, normal4);
    bitonicSort4(
        heightStat0, heightStat1, heightStat2, heightStat3,
        heightStatInit1, heightStatInit2, heightStatInit3, heightStatInit4,
        false);
    quaternion conjRot = conj(rotation);
    if (surfaceInd == 0)
        NORMAL = rotate(quaternion(-heightStat0.gba, 1.0), conjRot).xyz;
    else if (surfaceInd == 1)
        NORMAL = rotate(quaternion(-heightStat1.gba, 1.0), conjRot).xyz;
    else if (surfaceInd == 2)
        NORMAL = rotate(quaternion(-heightStat2.gba, 1.0), conjRot).xyz;
    else if (surfaceInd == 3)
        NORMAL = rotate(quaternion(-heightStat3.gba, 1.0), conjRot).xyz;
}