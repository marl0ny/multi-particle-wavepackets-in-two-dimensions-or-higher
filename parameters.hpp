#include "gl_wrappers.hpp"

namespace sim_2d {

#ifndef _PARAMETERS_
#define _PARAMETERS_

struct Button {};

struct UploadImage {};

typedef std::string Label;

typedef bool BoolRecord;

typedef std::vector<std::string> EntryBoxes;

struct SelectionList {
    int selected;
    std::vector<std::string> options;
};

struct LineDivider {};

struct NotUsed {};

struct SimParams {
    int stepsPerFrame = (int)(1);
    float dt = (float)(0.08F);
    float t = (float)(0.0F);
    float c = (float)(137.036F);
    float brightness = (float)(0.1F);
    float potentialBrightness = (float)(1.0F);
    float hbar = (float)(1.0F);
    LineDivider lineDiv1 = LineDivider{};
    Label sliderSetWaveFuncTitle = Label{};
    float m1 = (float)(1.0F);
    float m2 = (float)(1.0F);
    Vec2 pos1 = (Vec2)(Vec2 {.ind={0.25, 0.25}});
    Vec2 pos2 = (Vec2)(Vec2 {.ind={0.75, 0.25}});
    Vec2 momentum1 = (Vec2)(Vec2 {.ind={0.0, 0.0}});
    Vec2 momentum2 = (Vec2)(Vec2 {.ind={0.0, 0.0}});
    float sigma1 = (float)(0.025F);
    float sigma2 = (float)(0.025F);
    Button enterWaveFunc = Button{};
    LineDivider lineDiv2 = LineDivider{};
    int log2TexWidth = (int)(5);
    Vec2 sliceCoord = (Vec2)(Vec2 {.ind={0.5, 0.5}});
    IVec2 sliceInd = (IVec2)(IVec2 {.ind={1, 3}});
    IVec2 sampleInd = (IVec2)(IVec2 {.ind={0, 2}});
    enum {
        STEPS_PER_FRAME=0,
        DT=1,
        T=2,
        C=3,
        BRIGHTNESS=4,
        POTENTIAL_BRIGHTNESS=5,
        HBAR=6,
        LINE_DIV1=7,
        SLIDER_SET_WAVE_FUNC_TITLE=8,
        M1=9,
        M2=10,
        POS1=11,
        POS2=12,
        MOMENTUM1=13,
        MOMENTUM2=14,
        SIGMA1=15,
        SIGMA2=16,
        ENTER_WAVE_FUNC=17,
        LINE_DIV2=18,
        LOG2_TEX_WIDTH=19,
        SLICE_COORD=20,
        SLICE_IND=21,
        SAMPLE_IND=22,
    };
    void set(int enum_val, Uniform val) {
        switch(enum_val) {
            case STEPS_PER_FRAME:
            stepsPerFrame = val.i32;
            break;
            case DT:
            dt = val.f32;
            break;
            case T:
            t = val.f32;
            break;
            case C:
            c = val.f32;
            break;
            case BRIGHTNESS:
            brightness = val.f32;
            break;
            case POTENTIAL_BRIGHTNESS:
            potentialBrightness = val.f32;
            break;
            case HBAR:
            hbar = val.f32;
            break;
            case M1:
            m1 = val.f32;
            break;
            case M2:
            m2 = val.f32;
            break;
            case POS1:
            pos1 = val.vec2;
            break;
            case POS2:
            pos2 = val.vec2;
            break;
            case MOMENTUM1:
            momentum1 = val.vec2;
            break;
            case MOMENTUM2:
            momentum2 = val.vec2;
            break;
            case SIGMA1:
            sigma1 = val.f32;
            break;
            case SIGMA2:
            sigma2 = val.f32;
            break;
            case LOG2_TEX_WIDTH:
            log2TexWidth = val.i32;
            break;
            case SLICE_COORD:
            sliceCoord = val.vec2;
            break;
            case SLICE_IND:
            sliceInd = val.ivec2;
            break;
            case SAMPLE_IND:
            sampleInd = val.ivec2;
            break;
        }
    }
    Uniform get(int enum_val) const {
        switch(enum_val) {
            case STEPS_PER_FRAME:
            return {(int)stepsPerFrame};
            case DT:
            return {(float)dt};
            case T:
            return {(float)t};
            case C:
            return {(float)c};
            case BRIGHTNESS:
            return {(float)brightness};
            case POTENTIAL_BRIGHTNESS:
            return {(float)potentialBrightness};
            case HBAR:
            return {(float)hbar};
            case M1:
            return {(float)m1};
            case M2:
            return {(float)m2};
            case POS1:
            return {(Vec2)pos1};
            case POS2:
            return {(Vec2)pos2};
            case MOMENTUM1:
            return {(Vec2)momentum1};
            case MOMENTUM2:
            return {(Vec2)momentum2};
            case SIGMA1:
            return {(float)sigma1};
            case SIGMA2:
            return {(float)sigma2};
            case LOG2_TEX_WIDTH:
            return {(int)log2TexWidth};
            case SLICE_COORD:
            return {(Vec2)sliceCoord};
            case SLICE_IND:
            return {(IVec2)sliceInd};
            case SAMPLE_IND:
            return {(IVec2)sampleInd};
        }
        return Uniform(0);
    }
    void set(int enum_val, int index, std::string val) {
        switch(enum_val) {
            case SLIDER_SET_WAVE_FUNC_TITLE:
            sliderSetWaveFuncTitle = val;
            break;
        }
    }
};
#endif
}
