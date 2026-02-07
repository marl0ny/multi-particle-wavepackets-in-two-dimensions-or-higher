#include "simulation.hpp"

static const double PI = 3.141592653589793;

static const std::vector<float> QUAD_VERTICES = {
    -1.0, -1.0, 0.0, -1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, -1.0, 0.0};
static const std::vector<int> QUAD_ELEMENTS = {0, 1, 2, 0, 2, 3};
static WireFrame get_quad_wire_frame() {
    return WireFrame(
        {{"position", Attribute{
            3, GL_FLOAT, false,
            0, 0}}},
        QUAD_VERTICES, QUAD_ELEMENTS,
        WireFrame::TRIANGLES
    );
}

static unsigned int power2(unsigned int pow) {
    if (pow == 0)
        return 1;
    unsigned int res = 2;
    for (; pow > 1; pow--)
        res *= 2;
    return res;
}

static Vec4 get_dimensions_4d(int log2_size) {
    int size = power2(log2_size);
    float f_width = (float)size;
    return Vec4{.ind{f_width, f_width, f_width, f_width}};
}

static IVec4 get_texel_dimensions_4d(int log2_size) {
    int size = power2(log2_size);
    return IVec4{.ind{size, size, size, size}};
}

Frames::Frames(
    const TextureParams &default_tex_params, 
    const SimParams &params): 
    sim_params({
        .format=GL_R32F,
        .width=power2(2*params.log2TexWidth),
        .height=power2(2*params.log2TexWidth),
        .generate_mipmap=1,
        .min_filter=default_tex_params.min_filter,
        .mag_filter=default_tex_params.mag_filter,
        .wrap_s=GL_REPEAT,
        .wrap_t=GL_REPEAT
    }),
    view_params(default_tex_params),
    slice_xy_params({
        .format=GL_R32F,
        .width=power2(params.log2TexWidth),
        .height=power2(params.log2TexWidth),
        .generate_mipmap=1,
        .min_filter=default_tex_params.min_filter,
        .mag_filter=default_tex_params.mag_filter,
        .wrap_s=GL_REPEAT,
        .wrap_t=GL_REPEAT
    }),
    slice_zw_params({
        .format=GL_R32F,
        .width=power2(params.log2TexWidth),
        .height=power2(params.log2TexWidth),
        .generate_mipmap=1,
        .min_filter=default_tex_params.min_filter,
        .mag_filter=default_tex_params.mag_filter,
        .wrap_s=GL_REPEAT,
        .wrap_t=GL_REPEAT
    }),
    re_psi {Quad(sim_params), Quad(sim_params)},
    im_psi {Quad(sim_params), Quad(sim_params)},
    sim_tmp(Quad(sim_params)),
    potential(Quad(sim_params)),
    re_psi_slice{Quad(slice_xy_params), Quad(slice_xy_params)},
    im_psi_slice{Quad(slice_xy_params), Quad(slice_xy_params)},
    potential_slice(Quad(slice_xy_params)),
    render(RenderTarget{view_params}),
    quad_wire_frame(get_quad_wire_frame())
    {
}

void Frames::change_simulation_dimensions(const SimParams &params) {
    this->sim_params.width = power2(2*params.log2TexWidth);
    this->sim_params.height = power2(2*params.log2TexWidth);
    this->re_psi[0].reset(this->sim_params);
    this->re_psi[1].reset(this->sim_params);
    this->im_psi[0].reset(this->sim_params);
    this->im_psi[1].reset(this->sim_params);
    this->sim_tmp.reset(this->sim_params);
    this->potential.reset(this->sim_params);

}

Programs::Programs() {
    this->scale = Quad::make_program_from_path(
        "./shaders/util/scale.frag"
    );
    this->add2 = Quad::make_program_from_path(
        "./shaders/util/add2.frag"
    );
    this->add4_r = Quad::make_program_from_path(
        "./shaders/util/add4-r.frag"
    );
    this->rgb_combine = Quad::make_program_from_path(
        "./shaders/util/rgb-combine.frag"
    );
    this->wave_packet = Quad::make_program_from_path(
         "./shaders/wavepacket/gaussian.frag"
    );
    this->interaction = Quad::make_program_from_path(
        "./shaders/potentials/coulomb-interaction-like.frag"
    );
    this->time_step = Quad::make_program_from_path(
        "./shaders/time-step/leapfrog.frag"
    );
    this->transpose = Quad::make_program_from_path(
        "./shaders/util/transpose-hypercube.frag"
    );
    this->norm_squared = Quad::make_program_from_path(
        "./shaders/util/norm-squared.frag"
    );
    this->slice = Quad::make_program_from_path(
        "./shaders/util/slice-of-4d.frag"
    );
    this->visualization1 = Quad::make_program_from_path(
        "./shaders/visualization/d-color-wfn-gscale-pot.frag"
    );
    this->perp_lines = Quad::make_program_from_path(
        "./shaders/util/perp-lines.frag"
    );
}

void Simulation::initial_wave_function(
    const SimParams &params,
    Vec2 x1, Vec2 x2, Vec2 p1, Vec2 p2) {
    IVec4 tex_d_4d = get_texel_dimensions_4d(params.log2TexWidth);
    printf("%d, %d, %d, %d\n",
        tex_d_4d[0], tex_d_4d[1], tex_d_4d[2], tex_d_4d[3]);
    Vec4 d_4d = get_dimensions_4d(params.log2TexWidth);
    printf("%g, %g, %g, %g\n",
        d_4d[0], d_4d[1], d_4d[2], d_4d[3]);
    float symm_factor = 0.0;
    IVec2 indices1 = {.ind{0, 2}};
    IVec2 indices2 = {.ind{1, 3}};
    float amplitude1 = 1.0, amplitude2 = 1.0;
    Vec2 sigma1 = {.x=params.sigma1, .y=params.sigma1};
    Vec2 sigma2 = {.x=params.sigma2, .y=params.sigma2};
    Vec2 tex_offset1 = Vec2{.x=x1.x, .y=x1.y};
    Vec2 tex_offset2 = Vec2{.x=x2.x, .y=x2.y};
    Vec2 wave_number1 = Vec2{.x=p1.x, .y=p1.y};
    Vec2 wave_number2 = Vec2{.x=p2.x, .y=p2.y};
    m_frames.re_psi[m_re_indices[0]].draw(
        m_programs.wave_packet,
        {
            {"texelDimensions4D", tex_d_4d},
            {"symmetryFactor", symm_factor},
            {"indices1", indices1},
            {"indices2", indices2},
            {"amplitude1", amplitude1},
            {"amplitude2", amplitude2},
            {"sigma1", sigma1},
            {"sigma2", sigma2},
            {"texOffset1", tex_offset1},
            {"texOffset2", tex_offset2},
            {"waveNumber1", wave_number1},
            {"waveNumber2", wave_number2},
            {"swapRealImaginaryParts", int(0)}
        }
    );
    m_frames.im_psi[m_im_indices[0]].draw(
        m_programs.wave_packet,
        {
            {"texelDimensions4D", tex_d_4d},
            {"symmetryFactor", symm_factor},
            {"indices1", indices1},
            {"indices2", indices2},
            {"amplitude1", amplitude1},
            {"amplitude2", amplitude2},
            {"sigma1", sigma1},
            {"sigma2", sigma2},
            {"texOffset1", tex_offset1},
            {"texOffset2", tex_offset2},
            {"waveNumber1", wave_number1},
            {"waveNumber2", wave_number2},
            {"swapRealImaginaryParts", int(1)}
        }
    );
}

void Simulation::initial_conditions(
    const SimParams &params,
    Vec2 x1, Vec2 x2, Vec2 p1, Vec2 p2){
    this->m_step_counter = 0;
    this->m_re_indices[0] = 0;
    this->m_re_indices[1] = 1;
    this->m_im_indices[0] = 0;
    this->m_im_indices[1] = 1;
    this->initial_wave_function(params, x1, x2, p1, p2);
    this->step(params);
}

void Simulation::change_simulation_dimensions(
    const SimParams &params
) {
    this->m_frames.change_simulation_dimensions(params);
    Vec4 d_4d = get_dimensions_4d(params.log2TexWidth);
    IVec4 tex_d_4d = get_texel_dimensions_4d(params.log2TexWidth);
    this->initial_conditions(params,
        params.pos1, params.pos2, 
        {.x=params.momentum1.x*float(tex_d_4d[0]), 
         .y=params.momentum1.y*float(tex_d_4d[2])}, 
        {.x=params.momentum2.x*float(tex_d_4d[1]),
         .y=params.momentum2.y*float(tex_d_4d[3])});
    IVec2 indices1 = {.ind{0, 2}};
    IVec2 indices2 = {.ind{1, 3}};
    m_frames.potential.draw(
        m_programs.interaction,
        {
            {"texelDimensions4D", tex_d_4d},
            {"dimensions4D", d_4d},
            {"largestAllowedPotentialValue", float(0.5*params.hbar*PI/params.dt)},
            {"indices1", indices1},
            {"indices2", indices2}
        }
    );
}

Simulation::Simulation(
    const TextureParams &default_tex_params,
    const SimParams &params): 
    m_programs(Programs()),
    m_frames(default_tex_params, params) {
    Vec4 d_4d = get_dimensions_4d(params.log2TexWidth);
    IVec4 tex_d_4d = get_texel_dimensions_4d(params.log2TexWidth);
    this->initial_conditions(
        params, 
        params.pos1, params.pos2,
        {.x=params.momentum1.x*float(tex_d_4d[0]), 
             .y=params.momentum1.y*float(tex_d_4d[2])}, 
        {.x=params.momentum2.x*float(tex_d_4d[1]),
             .y=params.momentum2.y*float(tex_d_4d[3])});
    IVec2 indices1 = {.ind{0, 2}};
    IVec2 indices2 = {.ind{1, 3}};
    m_frames.potential.draw(
        m_programs.interaction,
        {
            {"texelDimensions4D", tex_d_4d},
            {"dimensions4D", d_4d},
            {"largestAllowedPotentialValue", float(0.5*params.hbar*PI/params.dt)},
            {"indices1", indices1},
            {"indices2", indices2}
        }
    );
}

void Simulation::step(const SimParams &params) {
    Vec4 d_4d = get_dimensions_4d(params.log2TexWidth);
    IVec4 tex_d_4d = get_texel_dimensions_4d(params.log2TexWidth);
    Vec4 mass_indices {.ind{params.m2, params.m1, params.m2, params.m1}};
    m_frames.im_psi[m_im_indices[1]].draw(
        m_programs.time_step,
        {
            {"rePsiTex", &m_frames.re_psi[m_re_indices[0]]},
            {"imPsiTex", &m_frames.im_psi[m_im_indices[0]]},
            {"potentialTex", &m_frames.potential},
            {"isRealStep", int(0)},
            {"hbar", params.hbar},
            {"massIndices", mass_indices},
            {"dt", ((this->m_step_counter == 0)? 0.5F: 1.0F)*params.dt},
            {"dimensions4D", d_4d},
            {"texelDimensions4D", tex_d_4d}
        }
    );
    std::swap(m_im_indices[0], m_im_indices[1]);
    m_frames.re_psi[m_re_indices[1]].draw(
        m_programs.time_step,
        {
            {"rePsiTex", &m_frames.re_psi[m_re_indices[0]]},
            {"imPsiTex", &m_frames.im_psi[m_im_indices[0]]},
            {"potentialTex", &m_frames.potential},
            {"isRealStep", int(1)},
            {"hbar", params.hbar},
            {"massIndices", mass_indices},
            {"dt", params.dt},
            {"dimensions4D", d_4d},
            {"texelDimensions4D", tex_d_4d}
        }
    );
    std::swap(m_re_indices[0], m_re_indices[1]);
    this->m_step_counter++;
}

const RenderTarget &Simulation::view(const SimParams &params) {
    Vec4 d_4d = get_dimensions_4d(params.log2TexWidth);
    IVec4 tex_d_4d = get_texel_dimensions_4d(params.log2TexWidth);
    IVec2 slice_coordinates = {.ind{
        int(params.sliceCoord[0]*float(tex_d_4d[0])),
        int(params.sliceCoord[1]*float(tex_d_4d[2]))
    }};
    /* for (int i = 0; i < 2; i++) {
        this->m_frames.re_psi_slice[i].draw(
            m_programs.slice,
            {
                {"tex", &this->m_frames.re_psi[i]},
                {"texelDimensions4D", tex_d_4d},
                {"sliceCoordinates", slice_coordinates},
                {"sliceIndices", params.sliceInd},
                {"sampleIndices", params.sampleInd}
            }
        );
        this->m_frames.im_psi_slice[i].draw(
            m_programs.slice,
            {
                {"tex", &this->m_frames.im_psi[i]},
                {"texelDimensions4D", tex_d_4d},
                {"sliceCoordinates", slice_coordinates},
                {"sliceIndices", params.sliceInd},
                {"sampleIndices", params.sampleInd}
            }
        );
    }
    this->m_frames.potential_slice.draw(
        m_programs.slice,
        {
            {"tex", &this->m_frames.potential},
            {"texelDimensions4D", tex_d_4d},
            {"sliceCoordinates", slice_coordinates},
            {"sliceIndices", params.sliceInd},
            {"sampleIndices", params.sampleInd}
        }
    );
    this->m_frames.render.draw(
        m_programs.visualization1,
        {
            {"reTex", &m_frames.re_psi_slice[0]},
            {"imTex1", &m_frames.im_psi_slice[0]},
            {"imTex2", &m_frames.im_psi_slice[1]},
            {"waveFunctionBrightness", params.brightness},
            {"potentialBrightness", params.potentialBrightness},
            {"potentialTex", &m_frames.potential_slice}
        },
        m_frames.quad_wire_frame
    );*/
    this->m_frames.render.draw(
        m_programs.visualization1,
        {
            {"reTex", &m_frames.re_psi[0]},
            {"imTex1", &m_frames.im_psi[0]},
            {"imTex2", &m_frames.im_psi[1]},
            {"waveFunctionBrightness", params.brightness},
            {"potentialBrightness", params.potentialBrightness},
            {"potentialTex", &m_frames.potential}
        },
        m_frames.quad_wire_frame
    );
    return this->m_frames.render;
}