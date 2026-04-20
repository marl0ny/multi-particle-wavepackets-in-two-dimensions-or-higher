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
    float brightness1 = (float)(0.1F);
    float brightness2 = (float)(0.1F);
    float brightness3 = (float)(0.1F);
    float potentialBrightness = (float)(1.0F);
    float hbar = (float)(1.0F);
    float interactionStrength = (float)(1.0F);
    bool applyAbsorbingBoundaries = (bool)(true);
    SelectionList mouseUsage = SelectionList{0, {"None", "Visualize ψ(x1, y1; (x2, y2)=cursor location)", "———— ψ((x1, y1)=cursor location; x2, y2)"}};
    LineDivider lineDiv1 = LineDivider{};
    Label sliderSetWaveFuncTitle = Label{};
    bool showInitialWavePacketAverages = (bool)(true);
    SelectionList symmetrySelection = SelectionList{0, {"None", "Symmetric", "Antisymmetric"}};
    int symmetry = (int)(0);
    float m1 = (float)(1.0F);
    float m2 = (float)(1.0F);
    Vec2 pos1 = (Vec2)(Vec2 {.ind={0.25, 0.75}});
    Vec2 momentum1 = (Vec2)(Vec2 {.ind={0.075, -0.075}});
    float sigma1 = (float)(0.045F);
    Vec2 pos2 = (Vec2)(Vec2 {.ind={0.75, 0.25}});
    Vec2 momentum2 = (Vec2)(Vec2 {.ind={-0.075, 0.075}});
    float sigma2 = (float)(0.045F);
    Button enterWaveFunc = Button{};
    LineDivider lineDiv2 = LineDivider{};
    int maxLog2TexWidth = (int)(7);
    int log2TexWidth = (int)(6);
    IVec2 sliceInd = (IVec2)(IVec2 {.ind={1, 3}});
    IVec2 sampleInd = (IVec2)(IVec2 {.ind={0, 2}});
    float dt = (float)(0.06F);
    float t = (float)(0.0F);
    float c = (float)(137.036F);
    SelectionList presetPotentialDropdown = SelectionList{0, {"0", "amp*((x/width)^2 + (y/height)^2)", "3.8*(step(-y^2+(height*0.04*s1)^2)+step(y^2-(height*0.06*s2)^2))*step(-x^2+(width*0.04*w)^2)", "1.0/sqrt(x^2+y^2)+1.0/sqrt((x-0.25*width)^2+(y-0.25*height)^2)", "0.5*(x*cos(w*t/200)/width + y*sin(w*t/200)/height)", "0.5*(tanh(75.0*(((x/width)^2+(y/height)^2)^0.5-0.45))+1.0)"}};
    EntryBoxes userTextEntry = EntryBoxes{"0"};
    LineDivider lineDiv3 = LineDivider{};
    bool show3D = (bool)(false);
    float height1 = (float)(1.0F);
    float height2 = (float)(1.0F);
    float height3 = (float)(1.0F);
    float potentialHeight = (float)(1.0F);
    enum {
        STEPS_PER_FRAME=0,
        BRIGHTNESS1=1,
        BRIGHTNESS2=2,
        BRIGHTNESS3=3,
        POTENTIAL_BRIGHTNESS=4,
        HBAR=5,
        INTERACTION_STRENGTH=6,
        APPLY_ABSORBING_BOUNDARIES=7,
        MOUSE_USAGE=8,
        LINE_DIV1=9,
        SLIDER_SET_WAVE_FUNC_TITLE=10,
        SHOW_INITIAL_WAVE_PACKET_AVERAGES=11,
        SYMMETRY_SELECTION=12,
        SYMMETRY=13,
        M1=14,
        M2=15,
        POS1=16,
        MOMENTUM1=17,
        SIGMA1=18,
        POS2=19,
        MOMENTUM2=20,
        SIGMA2=21,
        ENTER_WAVE_FUNC=22,
        LINE_DIV2=23,
        MAX_LOG2_TEX_WIDTH=24,
        LOG2_TEX_WIDTH=25,
        SLICE_IND=26,
        SAMPLE_IND=27,
        DT=28,
        T=29,
        C=30,
        PRESET_POTENTIAL_DROPDOWN=31,
        USER_TEXT_ENTRY=32,
        LINE_DIV3=33,
        SHOW3_D=34,
        HEIGHT1=35,
        HEIGHT2=36,
        HEIGHT3=37,
        POTENTIAL_HEIGHT=38,
    };
    void set(int enum_val, Uniform val) {
        switch(enum_val) {
            case STEPS_PER_FRAME:
            stepsPerFrame = val.i32;
            break;
            case BRIGHTNESS1:
            brightness1 = val.f32;
            break;
            case BRIGHTNESS2:
            brightness2 = val.f32;
            break;
            case BRIGHTNESS3:
            brightness3 = val.f32;
            break;
            case POTENTIAL_BRIGHTNESS:
            potentialBrightness = val.f32;
            break;
            case HBAR:
            hbar = val.f32;
            break;
            case INTERACTION_STRENGTH:
            interactionStrength = val.f32;
            break;
            case APPLY_ABSORBING_BOUNDARIES:
            applyAbsorbingBoundaries = val.b32;
            break;
            case SHOW_INITIAL_WAVE_PACKET_AVERAGES:
            showInitialWavePacketAverages = val.b32;
            break;
            case SYMMETRY:
            symmetry = val.i32;
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
            case MOMENTUM1:
            momentum1 = val.vec2;
            break;
            case SIGMA1:
            sigma1 = val.f32;
            break;
            case POS2:
            pos2 = val.vec2;
            break;
            case MOMENTUM2:
            momentum2 = val.vec2;
            break;
            case SIGMA2:
            sigma2 = val.f32;
            break;
            case MAX_LOG2_TEX_WIDTH:
            maxLog2TexWidth = val.i32;
            break;
            case LOG2_TEX_WIDTH:
            log2TexWidth = val.i32;
            break;
            case SLICE_IND:
            sliceInd = val.ivec2;
            break;
            case SAMPLE_IND:
            sampleInd = val.ivec2;
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
            case SHOW3_D:
            show3D = val.b32;
            break;
            case HEIGHT1:
            height1 = val.f32;
            break;
            case HEIGHT2:
            height2 = val.f32;
            break;
            case HEIGHT3:
            height3 = val.f32;
            break;
            case POTENTIAL_HEIGHT:
            potentialHeight = val.f32;
            break;
        }
    }
    Uniform get(int enum_val) const {
        switch(enum_val) {
            case STEPS_PER_FRAME:
            return {(int)stepsPerFrame};
            case BRIGHTNESS1:
            return {(float)brightness1};
            case BRIGHTNESS2:
            return {(float)brightness2};
            case BRIGHTNESS3:
            return {(float)brightness3};
            case POTENTIAL_BRIGHTNESS:
            return {(float)potentialBrightness};
            case HBAR:
            return {(float)hbar};
            case INTERACTION_STRENGTH:
            return {(float)interactionStrength};
            case APPLY_ABSORBING_BOUNDARIES:
            return {(bool)applyAbsorbingBoundaries};
            case SHOW_INITIAL_WAVE_PACKET_AVERAGES:
            return {(bool)showInitialWavePacketAverages};
            case SYMMETRY:
            return {(int)symmetry};
            case M1:
            return {(float)m1};
            case M2:
            return {(float)m2};
            case POS1:
            return {(Vec2)pos1};
            case MOMENTUM1:
            return {(Vec2)momentum1};
            case SIGMA1:
            return {(float)sigma1};
            case POS2:
            return {(Vec2)pos2};
            case MOMENTUM2:
            return {(Vec2)momentum2};
            case SIGMA2:
            return {(float)sigma2};
            case MAX_LOG2_TEX_WIDTH:
            return {(int)maxLog2TexWidth};
            case LOG2_TEX_WIDTH:
            return {(int)log2TexWidth};
            case SLICE_IND:
            return {(IVec2)sliceInd};
            case SAMPLE_IND:
            return {(IVec2)sampleInd};
            case DT:
            return {(float)dt};
            case T:
            return {(float)t};
            case C:
            return {(float)c};
            case SHOW3_D:
            return {(bool)show3D};
            case HEIGHT1:
            return {(float)height1};
            case HEIGHT2:
            return {(float)height2};
            case HEIGHT3:
            return {(float)height3};
            case POTENTIAL_HEIGHT:
            return {(float)potentialHeight};
        }
        return Uniform(0);
    }
    void set(int enum_val, int index, std::string val) {
        switch(enum_val) {
            case SLIDER_SET_WAVE_FUNC_TITLE:
            sliderSetWaveFuncTitle = val;
            break;
            case USER_TEXT_ENTRY:
            userTextEntry[index] = val;
            break;
        }
    }
};
#endif
}
