#include "gl_wrappers.hpp"
#include "parameters.hpp"
#include <cstdint>

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
    Quad reductions[9];
    // std::vector<Quad> reductions;
    Quad slices[4];
    Quad potential_slice;
    RenderTarget render_tmp;
    RenderTarget render;
    WireFrame quad_wire_frame;
    WireFrame surface;
    Frames(const TextureParams &default_tex_params, const SimParams &params);
    void change_simulation_dimensions(const SimParams &params);
};

struct Programs {
    uint32_t scale, add2, add4_r, rgb_combine, uniform_color;
    uint32_t wave_packet, interaction;
    uint32_t time_step;
    uint32_t transpose_norm_squared, norm_squared, slice;
    uint32_t surface_mag_color_map; 
    uint32_t surface_domain_color, surface_single_color;
    uint32_t visualization1, visualization2, perp_lines;
    uint32_t reduce_4x4;
    uint32_t user_defined;
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
    void particle1_prob_view(Quad &dst, const SimParams &params);
    void particle2_prob_view(Quad &dst, const SimParams &params);
    void entire_wave_func_view(const SimParams &params);
    void wave_func_xy_slice_view(
        RenderTarget &dst, IVec2 slice_coordinates, const SimParams &params);
    public:
    Simulation(
        const TextureParams &default_tex_params, 
        const SimParams &params);
    void modify_interactive_potential(const SimParams &params);
    void initial_conditions(
        const SimParams &params,
        Vec2 x1, Vec2 x2, Vec2 p1, Vec2 p2);
    void change_simulation_dimensions(const SimParams &params);
    void add_user_defined_potential(
        const SimParams &params,
        unsigned int program,
        const std::map<std::string, float> &uniforms);
    void step(const SimParams &params);
    const RenderTarget &view(
        const SimParams &params, 
        const std::optional<Vec2> &hover);
    const RenderTarget
    &view(
        const SimParams &params, 
        const std::optional <Vec2> &hover, 
        ::Quaternion rotation, float scale);
};



#endif
