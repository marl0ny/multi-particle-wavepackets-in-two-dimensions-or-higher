
#include "parameters.hpp"

#ifndef _IMGUI_CONTROLS_
#define _IMGUI_CONTROLS_
using namespace sim_2d;

#include "gl_wrappers.hpp"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include <functional>
#include <set>

#include "parameters.hpp"

static std::function<void(int, Uniform)> s_sim_params_set;
static std::function<void(int, int, std::string)> s_sim_params_set_string;
static std::function<Uniform(int)> s_sim_params_get;
static std::function<void(int, std::string, float)> s_user_edit_set_value;
static std::function<float(int, std::string)> s_user_edit_get_value;
static std::function<std::string(int)>
    s_user_edit_get_comma_separated_variables;
static std::function<void(int)> s_button_pressed;
static std::function<void(int, int)> s_selection_set;
static std::function<void(int, std::string, float)>
    s_sim_params_set_user_float_param;

static ImGuiIO global_io;
static std::map<int, std::string> global_labels;

void edit_label_display(int c, std::string text_content) {
    global_labels[c] = text_content;
}

void display_parameters_as_sliders(
    int c, std::set<std::string> variables) {
    std::string string_val = "[";
    for (auto &e: variables)
        string_val += """ + e + "", ";
    string_val += "]";
    string_val 
        = "modifyUserSliders(" + std::to_string(c) + ", " + string_val + ");";
    // TODO
}

void start_gui(void *window) {
    bool show_controls_window = true;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void imgui_controls(void *void_params) {
    SimParams *params = (SimParams *)void_params;
    for (auto &e: global_labels)
        params->set(e.first, 0, e.second);
    if (ImGui::SliderInt("Steps/frame", &params->stepsPerFrame, 0, 20))
            s_sim_params_set(params->STEPS_PER_FRAME, params->stepsPerFrame);
    if (ImGui::SliderFloat("particle 1 prob. density brightness", &params->brightness1, 0.0, 5.0))
           s_sim_params_set(params->BRIGHTNESS1, params->brightness1);
    if (ImGui::SliderFloat("particle 2 prob. density brightness", &params->brightness2, 0.0, 5.0))
           s_sim_params_set(params->BRIGHTNESS2, params->brightness2);
    if (ImGui::SliderFloat("Wave function slice brightness", &params->brightness3, 0.0, 5.0))
           s_sim_params_set(params->BRIGHTNESS3, params->brightness3);
    if (ImGui::SliderFloat("Potential brightness", &params->potentialBrightness, 0.0, 1.0))
           s_sim_params_set(params->POTENTIAL_BRIGHTNESS, params->potentialBrightness);
    if (ImGui::SliderFloat("Interaction strength", &params->interactionStrength, -10.0, 10.0))
           s_sim_params_set(params->INTERACTION_STRENGTH, params->interactionStrength);
    ImGui::Checkbox("Apply absorbing boundaries", &params->applyAbsorbingBoundaries);
    if (ImGui::BeginMenu("Mouse usage")) {
        if (ImGui::MenuItem( "None"))
            s_selection_set(params->MOUSE_USAGE, 0);
        if (ImGui::MenuItem( "Visualize ψ(x1))
            s_selection_set(params->MOUSE_USAGE, 1);
        if (ImGui::MenuItem( y1; (x2))
            s_selection_set(params->MOUSE_USAGE, 2);
        if (ImGui::MenuItem( y2)=cursor location)"))
            s_selection_set(params->MOUSE_USAGE, 3);
        if (ImGui::MenuItem( "———— ψ((x1))
            s_selection_set(params->MOUSE_USAGE, 4);
        if (ImGui::MenuItem( y1)=cursor location; x2))
            s_selection_set(params->MOUSE_USAGE, 5);
        if (ImGui::MenuItem( y2)"))
            s_selection_set(params->MOUSE_USAGE, 6);
        ImGui::EndMenu();
    }
    ImGui::Text("--------------------------------------------------------------------------------");
    ImGui::Text("New ψ(x1, y1; x2, y2) controls:");
    ImGui::Checkbox("Show interactive GUI display for initial conditions", &params->showInitialWavePacketAverages);
    if (ImGui::BeginMenu("Particle interchange symmetry: ")) {
        if (ImGui::MenuItem( "None"))
            s_selection_set(params->SYMMETRY_SELECTION, 0);
        if (ImGui::MenuItem( "Symmetric"))
            s_selection_set(params->SYMMETRY_SELECTION, 1);
        if (ImGui::MenuItem( "Antisymmetric"))
            s_selection_set(params->SYMMETRY_SELECTION, 2);
        ImGui::EndMenu();
    }
    if (ImGui::SliderFloat("mass 1", &params->m1, 1.0, 100.0))
           s_sim_params_set(params->M1, params->m1);
    if (ImGui::SliderFloat("mass 2", &params->m2, 1.0, 100.0))
           s_sim_params_set(params->M2, params->m2);
    ImGui::Text("x1, y1 (expressed as fraction of domain side length)");
    if (ImGui::SliderFloat("pos1[0]", &params->pos1.ind[0], 0.0, 1.0))
           s_sim_params_set(params->POS1, params->pos1);
    if (ImGui::SliderFloat("pos1[1]", &params->pos1.ind[1], 0.0, 1.0))
           s_sim_params_set(params->POS1, params->pos1);
    ImGui::Text("momentum 1 (π radians)");
    if (ImGui::SliderFloat("momentum1[0]", &params->momentum1.ind[0], -0.25, 0.25))
           s_sim_params_set(params->MOMENTUM1, params->momentum1);
    if (ImGui::SliderFloat("momentum1[1]", &params->momentum1.ind[1], -0.25, 0.25))
           s_sim_params_set(params->MOMENTUM1, params->momentum1);
    if (ImGui::SliderFloat("size 1 (expressed as fraction of domain side length)", &params->sigma1, 0.02, 0.1))
           s_sim_params_set(params->SIGMA1, params->sigma1);
    ImGui::Text("x2, y2");
    if (ImGui::SliderFloat("pos2[0]", &params->pos2.ind[0], 0.0, 1.0))
           s_sim_params_set(params->POS2, params->pos2);
    if (ImGui::SliderFloat("pos2[1]", &params->pos2.ind[1], 0.0, 1.0))
           s_sim_params_set(params->POS2, params->pos2);
    ImGui::Text("momentum 2");
    if (ImGui::SliderFloat("momentum2[0]", &params->momentum2.ind[0], -0.25, 0.25))
           s_sim_params_set(params->MOMENTUM2, params->momentum2);
    if (ImGui::SliderFloat("momentum2[1]", &params->momentum2.ind[1], -0.25, 0.25))
           s_sim_params_set(params->MOMENTUM2, params->momentum2);
    if (ImGui::SliderFloat("size 2", &params->sigma2, 0.02, 0.1))
           s_sim_params_set(params->SIGMA2, params->sigma2);
    ImGui::Text("--------------------------------------------------------------------------------");
    if (ImGui::SliderInt("log2(domain side length) (log2TexWidth)", &params->log2TexWidth, 5, 7))
            s_sim_params_set(params->LOG2_TEX_WIDTH, params->log2TexWidth);
    if (ImGui::SliderFloat("Time step", &params->dt, 0.0, 0.08))
           s_sim_params_set(params->DT, params->dt);
    if (ImGui::BeginMenu("Preset V(x, y, t)")) {
        if (ImGui::MenuItem( "0"))
            s_selection_set(params->PRESET_POTENTIAL_DROPDOWN, 0);
        if (ImGui::MenuItem( "amp*((x/width)^2 + (y/height)^2)"))
            s_selection_set(params->PRESET_POTENTIAL_DROPDOWN, 1);
        if (ImGui::MenuItem( "3.8*(step(-y^2+(height*0.04*s1)^2)+step(y^2-(height*0.06*s2)^2))*step(-x^2+(width*0.04*w)^2)"))
            s_selection_set(params->PRESET_POTENTIAL_DROPDOWN, 2);
        if (ImGui::MenuItem( "1.0/sqrt(x^2+y^2)+1.0/sqrt((x-0.25*width)^2+(y-0.25*height)^2)"))
            s_selection_set(params->PRESET_POTENTIAL_DROPDOWN, 3);
        if (ImGui::MenuItem( "0.5*(x*cos(w*t/200)/width + y*sin(w*t/200)/height)"))
            s_selection_set(params->PRESET_POTENTIAL_DROPDOWN, 4);
        if (ImGui::MenuItem( "0.5*(tanh(75.0*(((x/width)^2+(y/height)^2)^0.5-0.45))+1.0)"))
            s_selection_set(params->PRESET_POTENTIAL_DROPDOWN, 5);
        ImGui::EndMenu();
    }
    ImGui::Text("--------------------------------------------------------------------------------");
    ImGui::Checkbox("Show 3D surface height views", &params->show3D);
    if (ImGui::SliderFloat("particle prob. density height", &params->height1, 0.1, 10.0))
           s_sim_params_set(params->HEIGHT1, params->height1);
    if (ImGui::SliderFloat("transparency 1", &params->transparency1, 0.05, 1.0))
           s_sim_params_set(params->TRANSPARENCY1, params->transparency1);
    if (ImGui::SliderFloat("transparency 2", &params->transparency2, 0.05, 1.0))
           s_sim_params_set(params->TRANSPARENCY2, params->transparency2);
    if (ImGui::SliderFloat("Wave function slice height", &params->height3, 0.05, 1.0))
           s_sim_params_set(params->HEIGHT3, params->height3);
    if (ImGui::SliderFloat("Wave function slice transparency", &params->transparency3, 0.05, 1.0))
           s_sim_params_set(params->TRANSPARENCY3, params->transparency3);
    if (ImGui::SliderFloat("Potential height", &params->potentialHeight, 0.0, 10.0))
           s_sim_params_set(params->POTENTIAL_HEIGHT, params->potentialHeight);
    ImGui::Checkbox("Show 3D cursor position", &params->show3DCursor);

}

bool outside_gui() {{
    return !global_io.WantCaptureMouse;
}}

void display_gui(void *data) {{
    global_io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    bool val = true;
    ImGui::Begin("Controls", &val);
    ImGui::Text("WIP AND INCOMPLETE");
    imgui_controls(data);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}}

#endif
