#include "gl_wrappers.hpp"
#include "parameters.hpp"

#ifndef _SIMULATION_
#define _SIMULATION_

using namespace sim_2d;

struct Frames {
    TextureParams sim_params;
    TextureParams view_params;
    TextureParams slice_xy_params;
    TextureParams slice_zw_params;
    Quad re_psi[2];
    Quad im_psi[2];
    Quad sim_tmp;
    Quad potential;
    Quad re_psi_slice[2];
    Quad im_psi_slice[2];
    Quad potential_slice;
    RenderTarget render;
    WireFrame quad_wire_frame;
    Frames(const TextureParams &default_tex_params, const SimParams &params);
    void change_simulation_dimensions(const SimParams &params);
};

struct Programs {
    uint32_t scale, add2, add4_r, rgb_combine;
    uint32_t wave_packet, interaction;
    uint32_t time_step;
    uint32_t transpose, norm_squared, slice;
    uint32_t visualization1, perp_lines;
    Programs();
};

class Simulation {
    Programs m_programs;
    Frames m_frames;
    int m_step_counter;
    int m_re_indices[2];
    int m_im_indices[2];
    void initial_wave_function(
        const SimParams &params,
        Vec2 x1, Vec2 x2, Vec2 p1, Vec2 p2);
    public:
    Simulation(
        const TextureParams &default_tex_params, 
        const SimParams &params);
    void initial_conditions(
        const SimParams &params,
        Vec2 x1, Vec2 x2, Vec2 p1, Vec2 p2);
    void change_simulation_dimensions(const SimParams &params);
    void step(const SimParams &params);
    const RenderTarget &view(const SimParams &params);
    const RenderTarget
    &view(SimParams &params, ::Quaternion rotation, float scale);
};



#endif
