#if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
#define texture2D texture
#else
#define texture texture2D
#endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif
    
#if __VERSION__ <= 120
varying vec4 HEIGHTS;
varying vec2 UV;
varying float SURFACE_IND;
varying vec3 FINAL_VERTEX_POSITION;
varying vec3 NORMAL;
#define fragColor gl_FragColor
#else
in vec4 HEIGHTS;
in vec2 UV;
in float SURFACE_IND;
in vec3 FINAL_VERTEX_POSITION;
in vec3 NORMAL;
out vec4 fragColor;
#endif

#define complex vec2

#define PI 3.141592653589793

const int SINGLE_VALUE = 0;
const int SCALAR_MAG = 1;
const int COPY_OVER = 2;
const int DOMAIN_COLOR_COMPLEX = 3;
const int DOMAIN_COLOR_COMPLEX_ABS_VAL = 4;

uniform sampler2D tex1;
uniform int drawType1;
uniform float brightness1;
uniform vec4 color1;

uniform sampler2D tex2;
uniform int drawType2;
uniform float brightness2;
uniform vec4 color2;

uniform sampler2D tex3;
uniform int drawType3;
uniform float brightness3;
uniform vec4 color3;

uniform sampler2D tex4;
uniform int drawType4;
uniform float brightness4;
uniform vec4 color4;

vec4 getSingleColorFragmentColor(vec4 color) {
    vec3 lightSourceLoc = vec3(0.0, 0.0, -3.0);
    vec3 vertexToLightSource = lightSourceLoc - FINAL_VERTEX_POSITION;
    float diffuse1 = max(dot(NORMAL, normalize(vertexToLightSource)), 0.0);
    float diffuse2 =  max(dot(-NORMAL, normalize(vertexToLightSource)), 0.0);
    // float diffuse2 = max(0.5*dot(NORMAL, vec3(0.0, 0.0, -1.0)), 0.0); 
    float diffuse = ((NORMAL.z < 0.0)? diffuse1: diffuse2);
    return vec4(color.rgb*(diffuse), color.a);
}

vec4 getMagColorFragmentColor(sampler2D tex, float brightness, vec4 color) {
    float ambient = 0.01;
    float r = texture2D(tex, UV)[0];
    float actBrightness = brightness*r;
    vec3 actColor = ambient + color.rgb*actBrightness;
    return vec4(actColor, color.a);
}


complex mul(complex w, complex z) {
    return complex(w.x*z.x - w.y*z.y, w.x*z.y + w.y*z.x);
}

vec3 argumentToColor(float argVal) {
    float maxCol = 1.0;
    float minCol = 50.0/255.0;
    float colRange = maxCol - minCol;
    if (argVal <= PI/3.0 && argVal >= 0.0) {
        return vec3(maxCol,
                    minCol + colRange*argVal/(PI/3.0), minCol);
    } else if (argVal > PI/3.0 && argVal <= 2.0*PI/3.0){
        return vec3(maxCol - colRange*(argVal - PI/3.0)/(PI/3.0),
                    maxCol, minCol);
    } else if (argVal > 2.0*PI/3.0 && argVal <= PI){
        return vec3(minCol, maxCol,
                    minCol + colRange*(argVal - 2.0*PI/3.0)/(PI/3.0));
    } else if (argVal < 0.0 && argVal > -PI/3.0){
        return vec3(maxCol, minCol,
                    minCol - colRange*argVal/(PI/3.0));
    } else if (argVal <= -PI/3.0 && argVal > -2.0*PI/3.0){
        return vec3(maxCol + (colRange*(argVal + PI/3.0)/(PI/3.0)),
                    minCol, maxCol);
    } else if (argVal <= -2.0*PI/3.0 && argVal >= -PI){
        return vec3(minCol,
                    minCol - (colRange*(argVal + 2.0*PI/3.0)/(PI/3.0)), 
                    maxCol);
    }
    else {
        return vec3(minCol, maxCol, maxCol);
    }
}

vec4 getComplexWithAbsValFragmentColor(
    sampler2D tex, float brightness, float alpha) {
    float re = texture2D(tex, UV)[0];
    float im = texture2D(tex, UV)[1];
    float absVal = texture2D(tex, UV)[2];
    complex z = complex(re, im);
    vec3 color = brightness*absVal*argumentToColor(atan(z.y, z.x));
    return vec4(color, alpha);
}

vec4 getFragmentColor(
    sampler2D tex, int drawType, float brightness, vec4 color) {
    if (drawType == SINGLE_VALUE) {
        return getSingleColorFragmentColor(color);
    } else if (drawType == SCALAR_MAG) {
        return getMagColorFragmentColor(tex, brightness, color);
    } else if (drawType == COPY_OVER) {
        return texture2D(tex, UV);
    } else if (drawType == DOMAIN_COLOR_COMPLEX_ABS_VAL) {
        return getComplexWithAbsValFragmentColor(tex, brightness, color.a);
    }
    return getMagColorFragmentColor(tex, brightness, color);
}

void discardIfValuesSimilar(float val, vec4 vals, int i1, int i2, int i3) {
    if (abs(val - vals[i1]) < 0.002 || 
        abs(val - vals[i2]) < 0.002 || 
        abs(val - vals[i3]) < 0.002)
        discard;
}

void main() {
    if (abs(FINAL_VERTEX_POSITION.x) > 2.0 || abs(FINAL_VERTEX_POSITION.y) > 2.0 ||
        abs(FINAL_VERTEX_POSITION.z) > 2.0)
        discard;
    if (int(SURFACE_IND) == 3) {
        fragColor = getFragmentColor(tex4, drawType4, brightness4, color4);
        // discardIfValuesSimilar(HEIGHTS[3], HEIGHTS, 0, 1, 2);
    } else if (int(SURFACE_IND) == 2) {
        fragColor = getFragmentColor(tex3, drawType3, brightness3, color3);
        // discardIfValuesSimilar(HEIGHTS[2], HEIGHTS, 0, 1, 3);
    } else if (int(SURFACE_IND) == 1) {
        fragColor = getFragmentColor(tex2, drawType2, brightness2, color2);
        // discardIfValuesSimilar(HEIGHTS[1], HEIGHTS, 0, 2, 3);
    } else if (int(SURFACE_IND) == 0) {
        fragColor = getFragmentColor(tex1, drawType1, brightness1, color1);
        // discardIfValuesSimilar(HEIGHTS[0], HEIGHTS, 1, 2, 3);
    }
}
