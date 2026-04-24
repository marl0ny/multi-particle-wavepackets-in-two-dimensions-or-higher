#include "simulation.hpp"
#include "multi_surface.hpp"
#include "reduce4d.hpp"
#include "surface.hpp"
#include "matrix.hpp"

#include <iostream>


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


static Vec3 get_line_plane_intersection(
    const Vec3 &line_start, const Vec3 &line_end,
    const Vec3 &plane_vector0, const Vec3 &plane_vector1, const Vec3 &offset) {
    Vec3 line_direction = (line_end - line_start).normalized();
    Vec3 b = line_start - offset;
    Matrix m ({
        {plane_vector0.x, plane_vector1.x, -line_direction.x},
        {plane_vector0.y, plane_vector1.y, -line_direction.y},
        {plane_vector0.z, plane_vector1.z, -line_direction.z}});
    auto b_vec = std::vector<double>{b[0], b[1], b[2]};
    std::vector<double> solution = m.solve(b_vec);
    return offset 
        + plane_vector0*float(solution[0]) + plane_vector1*float(solution[1]);
}

static std::vector<Vec3> line_from_screen_cursor(
    Quaternion rot, float scale, Vec2 screen_cursor_pos) {
    screen_cursor_pos = 2.0*(screen_cursor_pos - Vec2{.ind{0.5, 0.5}});
    Quaternion cursor_pos0_3d {
        .real=1.0,
        .i=screen_cursor_pos.x, .j=screen_cursor_pos.y, .k=-1.0
    };
    Quaternion cursor_pos1_3d {
        .real=1.0,
        .i=screen_cursor_pos.x, .j=screen_cursor_pos.y, .k=1.0
    };
    cursor_pos0_3d = rotate(cursor_pos0_3d, rot.conj())/scale;
    cursor_pos1_3d = rotate(cursor_pos1_3d, rot.conj())/scale;
    Vec3 cursor_pos0_3d_v {.ind={
        cursor_pos0_3d.i, cursor_pos0_3d.j, cursor_pos0_3d.k
    }};
    Vec3 cursor_pos1_3d_v {.ind={
        cursor_pos1_3d.i, cursor_pos1_3d.j, cursor_pos1_3d.k
    }};
    return {cursor_pos0_3d_v, cursor_pos1_3d_v};
}

static Vec2 get_intersection_from_user_input(
    Quaternion rotation, float scale, Vec2 user_input_loc) {
    Vec3 plane_vector1 {.x=1.0, .y=0.0, .z=0.0};
    Vec3 plane_vector2 {.x=0.0, .y=1.0, .z=0.0};
    auto lines = line_from_screen_cursor(
        rotation, scale, user_input_loc);
    Vec3 intersection = get_line_plane_intersection(
        lines[0], lines[1],
        plane_vector1, plane_vector2,
        Vec3{.x=0.0, .y=0.0, .z=0.0});
    Vec2 location2d = Vec2{.x=intersection.x + 0.5F, .y=intersection.y + 0.5F};
    return location2d;
}


static WireFrame get_perp_lines_wire_frame(
    Vec2 intersection_position
) {
    std::vector<float> vertices = {
        0.0, intersection_position.y, 0.0,
        intersection_position.x, intersection_position.y, 0.0,
        1.0, intersection_position.y, 0.0,
        intersection_position.x, 0.0, 0.0,
        intersection_position.x, 1.0, 0.0
    };
    for (float &e: vertices)
        e = 2.0*e - 1.0;
    std::vector<int> elements = {
        0, 1, 1, 2, 3, 1, 1, 4
    };
    return WireFrame(
        {{"position", Attribute{
            3, GL_FLOAT, false,
            0, 0}}},
        vertices, elements, WireFrame::LINES
    );
}

static WireFrame get_circle_wire_frame(
    Vec2 position, float radius
) {
    std::vector<float> vertices {};
    std::vector<int> elements {};
    int n_points = 25;
    for (int i = 0; i < n_points; i++) {
        float angle = 2.0*PI*float(i)/n_points;
        vertices.push_back(position.x + radius*cos(angle));
        vertices.push_back(position.y + radius*sin(angle));
        vertices.push_back(0.0);
        elements.push_back(i);
        if (i < n_points - 1)
            elements.push_back(i + 1);
        else
            elements.push_back(0);
    }
    for (float &e: vertices)
        e = 2.0F*e - 1.0F;
    return WireFrame(
        {{"position", Attribute{
            3, GL_FLOAT, false,
            0, 0}}},
        vertices, elements,
        WireFrame::LINES
    );
}

static WireFrame get_arrow_wire_frame(
    Vec2 position, Vec2 direction
) {
    float c = 0.9659258262890683;
    float s = 0.25881904510252074;
    Vec2 line1 = Vec2{
        .x= 0.8F*(c*direction.x - s*direction.y),
        .y= 0.8F*(s*direction.x + c*direction.y)
    };
    Vec2 line2 = Vec2{
        .x= 0.8F*(+c*direction.x + s*direction.y),
        .y= 0.8F*(-s*direction.x + c*direction.y),
    };
    std::vector<float> vertices {
        position.x, position.y, 0.0,
        position.x + direction.x,
        position.y + direction.y, 0.0,
        position.x + line1.x, position.y + line1.y, 0.0,
        position.x + line2.x, position.y + line2.y, 0.0
    };
    for (float &e: vertices)
        e = 2.0F*e - 1.0F;
    std::vector<int> elements = {
        0, 1, 1, 2, 1, 3
    };
    return WireFrame(
        {{"position", Attribute{
            3, GL_FLOAT, false,
            0, 0}}},
        vertices, elements,
        WireFrame::LINES
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
        // .width=power2(params.maxLog2TexWidth),
        // .height=power2(params.maxLog2TexWidth),
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
        // .width=power2(params.maxLog2TexWidth),
        // .height=power2(params.maxLog2TexWidth),
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
    reductions{
        Quad(slice_zw_params), 
        Quad(slice_zw_params),
        Quad(slice_zw_params),
        Quad(slice_zw_params),
        Quad(slice_zw_params),
        Quad(slice_zw_params),
        Quad(slice_zw_params),
        Quad(slice_zw_params),
        Quad(slice_zw_params)},
    slices{Quad(slice_xy_params), Quad(slice_xy_params),
           Quad(slice_xy_params), Quad(slice_xy_params)},
    potential_slice(Quad(slice_xy_params)),
    render_tmp(RenderTarget{view_params}),
    render(RenderTarget{view_params}),
    quad_wire_frame(get_quad_wire_frame()),
    /* surface(
        get_surface_wireframe({.ind{512, 512}})
    ),*/
    surface(
        multi_surface::get_wireframe(
            {.ind{512, 512}}, 4)
    )
    {
    int max_allowed_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_allowed_texture_size);
    printf("Maximum texture size: %d.\n", max_allowed_texture_size);
    int sizes[4] = {
        1024, 4096, 
        16384, 65536};
    int max_allowed_size = sizes[0];
    for (int i = 1; sizes[i] <= max_allowed_texture_size; i++)
        max_allowed_size = sizes[i];
    printf("Maximum allowed texture size: %d.\n", max_allowed_texture_size);
    printf("Maximum used texture size: %d\n", max_allowed_size);
    square_reduce4d::initialize_sum_quads(
        this->reductions,
        sim_params, max_allowed_size/2, 32);
    // for (int i = 0; i < sizeof(this->reductions)/sizeof(Quad); i++)
    //     std::cout << this->reductions[i].width() << std::endl;
    // 2D simulaiton discretization side width, Actual texture width
    // 2^5 = 32, 1024
    // 2^6 = 64, 4096
    // 2^7 = 128, 16384 -- May not be supported
    // 2^8 = 256, 64536 -- Way too big! Not used.
    //
    // Sum quad texture widths:
    // 32 64 128 256 512 1024 2048 4096 8192 (16384)
    // 9 textures in total
    // printf(
    //     "Size of reduction sum quads: %d\n",
    //     (int)this->reductions.size());
}

void Frames::change_simulation_dimensions(const SimParams &params) {
    this->sim_params.width = power2(2*params.log2TexWidth);
    this->sim_params.height = power2(2*params.log2TexWidth);
    this->slice_xy_params.width = power2(params.log2TexWidth);
    this->slice_xy_params.height = power2(params.log2TexWidth);
    this->slice_zw_params.width = power2(params.log2TexWidth);
    this->slice_zw_params.height = power2(params.log2TexWidth);
    this->re_psi[0].reset(this->sim_params);
    this->re_psi[1].reset(this->sim_params);
    this->im_psi[0].reset(this->sim_params);
    this->im_psi[1].reset(this->sim_params);
    this->sim_tmp.reset(this->sim_params);
    this->potential.reset(this->sim_params);
    this->slices[0].reset(this->slice_xy_params);
    this->slices[1].reset(this->slice_xy_params);
    this->slices[2].reset(this->slice_xy_params);
    this->slices[3].reset(this->slice_xy_params);
    this->potential_slice.reset(this->slice_xy_params);

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
    this->uniform_color = Quad::make_program_from_path(
        "./shaders/util/uniform-color.frag"
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
    this->transpose_norm_squared = Quad::make_program_from_path(
        "./shaders/util/transpose-hypercube-norm-squared.frag"
    );
    this->norm_squared = Quad::make_program_from_path(
        "./shaders/util/norm-squared.frag"
    );
    this->slice = Quad::make_program_from_path(
        "./shaders/util/slice-of-4d.frag"
    );
    this->surface_mag_color_map = make_program_from_paths(
        "./shaders/surface/surface.vert",
        "./shaders/surface/mag-color-map.frag"
    );
    this->surface_domain_color = make_program_from_paths(
        "./shaders/surface/surface.vert",
        "./shaders/surface/mag-color-map.frag"
    );
    this->surface_single_color = make_program_from_paths(
        "./shaders/surface/surface.vert", 
        "./shaders/surface/single-color.frag"
    );
    this->four_surfaces = make_program_from_paths(
        "./shaders/surface/four-surfaces.vert",
        "./shaders/surface/four-surfaces.frag");
    this->visualization1 = Quad::make_program_from_path(
        "./shaders/visualization/d-color-wfn-gscale-pot.frag"
    );
    this->visualization2 = Quad::make_program_from_path(
        "./shaders/visualization/diff-particles-gscale-pot.frag"
    );
    this->perp_lines = Quad::make_program_from_path(
        "./shaders/util/perp-lines.frag"
    );
    this->reduce_4x4 = Quad::make_program_from_path(
        "./shaders/util/reduce-4x4.frag"
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
    float symm_factor = float(params.symmetry);
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

void Simulation::modify_interactive_potential(const SimParams &params) {
    Vec4 d_4d = get_dimensions_4d(params.log2TexWidth);
    IVec4 tex_d_4d = get_texel_dimensions_4d(params.log2TexWidth);
    IVec2 indices1 = {.ind{0, 2}};
    IVec2 indices2 = {.ind{1, 3}};
    m_frames.potential.draw(
        m_programs.interaction,
        {
            {"texelDimensions4D", tex_d_4d},
            {"dimensions4D", d_4d},
            {"interactionStrength", float(params.interactionStrength)},
            {"largestAllowedPotentialValue", float(0.5*params.hbar*PI/params.dt)},
            {"indices1", indices1},
            {"indices2", indices2}
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
            {"interactionStrength", float(params.interactionStrength)},
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
            {"interactionStrength", float(params.interactionStrength)},
            {"largestAllowedPotentialValue", float(0.5*params.hbar*PI/params.dt)},
            {"indices1", indices1},
            {"indices2", indices2}
        }
    );
}

void Simulation::add_user_defined_potential(
    const SimParams &params,
    unsigned int program, const std::map<std::string, float> &input_uniforms) {
    Vec4 d_4d = get_dimensions_4d(params.log2TexWidth);
    this->m_programs.user_defined = program;
    Uniforms uniforms;
    for (const auto &e: input_uniforms)
        uniforms.insert({e.first, Vec2{.ind{e.second, 0.0}}});
    if (input_uniforms.count("t") > 0)
        uniforms.at("t").vec2.x = params.t;
    else
        uniforms.insert({"t", Vec2{.ind{params.t, 0.0}}});
    uniforms.insert(
        {"width",
            Vec2{.ind{d_4d[0], 0.0}}});
    uniforms.insert(
        {"height",
            Vec2{.ind{d_4d[2], 0.0}}});
    enum outputModeSelect {
        MODE_4VECTOR_REAL_OR_COMPLEX=0,
        MODE_COMPLEX4=4
    };
    uniforms.insert(
        {"outputModeSelect",int(MODE_COMPLEX4)}
    );
    uniforms.insert(
        {"useRealPartOfExpression", int(1)});
    m_frames.potential_slice.draw(program, uniforms);
    
}

void Simulation::step(const SimParams &params) {
    Vec4 d_4d = get_dimensions_4d(params.log2TexWidth);
    IVec4 tex_d_4d = get_texel_dimensions_4d(params.log2TexWidth);
    Vec4 mass_indices {.ind{params.m1, params.m2, params.m1, params.m2}};
    m_frames.im_psi[m_im_indices[1]].draw(
        m_programs.time_step,
        {
            {"rePsiTex", &m_frames.re_psi[m_re_indices[0]]},
            {"imPsiTex", &m_frames.im_psi[m_im_indices[0]]},
            {"intPotentialTex", &m_frames.potential},
            {"extPotentialTex", &m_frames.potential_slice},
            {"isRealStep", int(0)},
            {"applyAbsorbingBoundaries", int(params.applyAbsorbingBoundaries)},
            {"hbar", params.hbar},
            {"massIndices", mass_indices},
            {"dt", ((this->m_step_counter == 0)? 0.5F: 1.0F)*params.dt},
            {"applyClampingToPotential", int(1)},
            {"potentialClampValues", 
                    Vec2{.ind{
                        float(-0.5*params.hbar*PI/params.dt), 
                        float(0.5*params.hbar*PI/params.dt)}}},
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
            {"intPotentialTex", &m_frames.potential},
            {"extPotentialTex", &m_frames.potential_slice},
            {"isRealStep", int(1)},
            {"applyAbsorbingBoundaries", int(params.applyAbsorbingBoundaries)},
            {"hbar", params.hbar},
            {"massIndices", mass_indices},
            {"dt", params.dt},
            {"applyClampingToPotential", int(1)},
            {"potentialClampValues", 
                    Vec2{.ind{
                        float(-0.5*params.hbar*PI/params.dt), 
                        float(0.5*params.hbar*PI/params.dt)}}},
            {"dimensions4D", d_4d},
            {"texelDimensions4D", tex_d_4d}
        }
    );
    std::swap(m_re_indices[0], m_re_indices[1]);
    this->m_step_counter++;
}

void Simulation::particle1_prob_view(Quad &dst, const SimParams &params) {
    this->m_frames.sim_tmp.draw(
        m_programs.norm_squared,
        {
            {"reTex", &m_frames.re_psi[0]},
            {"imTex1", &m_frames.im_psi[0]},
            {"imTex2", &m_frames.im_psi[1]}
        }
    );
    Quad *last = square_reduce4d::reduce(
        &this->m_frames.reductions[0], power2(params.log2TexWidth),
        (m_frames.view_params.mag_filter == GL_LINEAR)?
            m_programs.scale: m_programs.reduce_4x4, 
        m_frames.sim_tmp);
    std::cout << m_frames.reductions[(params.log2TexWidth + 2) - 1].width() << std::endl;
    dst.draw(
        m_programs.scale,
        {
            // {"tex", &m_frames.sim_tmp},
            {"tex", last},
            {"scale", 1.0F}
        }
    );
}

void Simulation::particle2_prob_view(Quad &dst, const SimParams &params) {
    this->m_frames.sim_tmp.draw(
        m_programs.transpose_norm_squared,
        {
            {"reTex", &m_frames.re_psi[0]},
            {"imTex1", &m_frames.im_psi[0]},
            {"imTex2", &m_frames.im_psi[1]},
            {"indices", IVec4{.ind{1, 0, 3, 2}}},
            {"texelDimensions4D", 
                    get_texel_dimensions_4d(params.log2TexWidth)},
        }
    );
    Quad *last = square_reduce4d::reduce(
        &this->m_frames.reductions[0],  power2(params.log2TexWidth),
        (m_frames.view_params.mag_filter == GL_LINEAR)?
            m_programs.scale: m_programs.reduce_4x4, 
        m_frames.sim_tmp);
    std::cout << m_frames.reductions[(params.log2TexWidth + 2) - 1].width() << std::endl;
    dst.draw(
        m_programs.scale,
        {
            // {"tex", &m_frames.sim_tmp},
            {"tex", last},
            {"scale", 1.0F}
        }
    );
}

void Simulation::wave_func_xy_slice_view(
    RenderTarget &dst, IVec2 slice_coordinates, const SimParams &params) {
    IVec4 tex_d_4d = get_texel_dimensions_4d(params.log2TexWidth);
    this->m_frames.slices[2].draw(
        m_programs.slice,
        {
            {"tex", &this->m_frames.re_psi[0]},
            {"texelDimensions4D", tex_d_4d},
            {"sliceCoordinates", slice_coordinates},
            {"sliceIndices", params.sliceInd},
            {"sampleIndices", params.sampleInd}
        }
    );
    for (int i = 0; i < 2; i++) {
        this->m_frames.slices[i].draw(
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
    dst.draw(
        m_programs.visualization1,
        {
            {"reTex", &m_frames.slices[2]},
            {"imTex1", &m_frames.slices[0]},
            {"imTex2", &m_frames.slices[1]},
            {"waveFunctionBrightness",params.brightness3},
            {"potentialBrightness", params.potentialBrightness},
            {"potentialTex", &m_frames.potential_slice}
        },
        m_frames.quad_wire_frame
    );
}

void Simulation::entire_wave_func_view(const SimParams &params) {
    this->m_frames.render.draw(
        m_programs.visualization1,
        {
            {"reTex", &m_frames.re_psi[0]},
            {"imTex1", &m_frames.im_psi[0]},
            {"imTex2", &m_frames.im_psi[1]},
            {"waveFunctionBrightness", params.brightness3},
            {"potentialBrightness", params.potentialBrightness},
            {"potentialTex", &m_frames.potential}
        },
        m_frames.quad_wire_frame
    );
}

enum class SurfaceOrdering {
    Y_ASCENDING, Y_DESCENDING, X_ASCENDING, X_DESCENDING  
};

static SurfaceOrdering get_ordering(::Quaternion rotation) {
    Quaternion x_basis {.i=1.0, .j=0.0, .k=0.0, .real=0.0};
    Quaternion y_basis {.i=0.0, .j=1.0, .k=0.0, .real=0.0};
    x_basis = ::rotate(x_basis, rotation);
    y_basis = ::rotate(y_basis, rotation);
    Vec3 x_vec = Vec3{.x=x_basis.i, x_basis.j, x_basis.k};
    Vec3 y_vec = Vec3{.x=y_basis.i, y_basis.j, y_basis.k};
    Vec3 camera_vec = Vec3{.x=0.0, 0.0, 1.0};
    float dot_val1 = ::dot(x_vec, camera_vec);
    float dot_val2 = ::dot(y_vec, camera_vec);
    if (abs(dot_val1) > abs(dot_val2))
        return (dot_val1 < 0)? 
            SurfaceOrdering::X_ASCENDING: 
            SurfaceOrdering::X_DESCENDING;
    else 
        return (dot_val2 < 0)?
            SurfaceOrdering::Y_ASCENDING:
            SurfaceOrdering::Y_DESCENDING;

}

const RenderTarget &Simulation::view(
    const SimParams &params, const std::optional<Vec2> &hover,
    ::Quaternion rotation, float scale
) {
    if (params.show3D) {
        int ordering;
        switch(get_ordering(rotation)) {
            case SurfaceOrdering::X_ASCENDING:
            ordering = 2;
            std::cout << "x ascending\n";
            break;
            case SurfaceOrdering::X_DESCENDING:
            ordering = -2;
            std::cout << "x descending\n";
            break;
            case SurfaceOrdering::Y_ASCENDING:
            ordering = 1;
            std::cout << "y ascending\n";
            break;
            case SurfaceOrdering::Y_DESCENDING:
            ordering = -1;
            std::cout << "y descending\n";
            break;
        }
        m_frames.render.clear();
        particle1_prob_view(this->m_frames.slices[0], params);
        particle2_prob_view(this->m_frames.slices[1], params);
        IVec2 screen_dimensions = {.ind{
            (int)m_frames.view_params.width,
            (int)m_frames.view_params.height
        }};
        glEnable(GL_DEPTH_TEST);
        // TODO: check the next line
        glDepthFunc(GL_LESS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        const int REAL_DATA_TYPE = 0;
        // const int COMPLEX_DATA_TYPE = 1;
        Uniforms vertex_uniforms = {
            {"heightTex1", &this->m_frames.slices[0]},
            {"heightDataType1", REAL_DATA_TYPE},
            {"heightScale1", 
                params.height1
                / (float(pow(2.0, 2.0*params.log2TexWidth))*120)},
            {"heightOffset1", 
                0.015F*(float(pow(2.0, 2.0*params.log2TexWidth))*120)
                 / (params.height1)
            },
            {"heightTex2", &this->m_frames.slices[1]},
            {"heightDataType2", REAL_DATA_TYPE},
            {"heightScale2", 
                params.height1
                / (float(pow(2.0, 2.0*params.log2TexWidth))*120)},
            {"heightOffset2",
                0.01F*(float(pow(2.0, 2.0*params.log2TexWidth))*120)
                / (params.height1)
            },
            {"heightTex3", &m_frames.potential_slice},
            {"heightDataType3", REAL_DATA_TYPE},
            {"heightScale3", params.potentialHeight/10.0F},
            {"heightTex4", &m_frames.potential_slice},
            {"heightDataType4", REAL_DATA_TYPE},
            {"heightScale4", params.potentialHeight/10.0F},
            {"scale", scale},
            {"rotation", rotation},
            {"screenDimensions", screen_dimensions},
            {"translate", Vec3{.ind{0.0, 0.0, 0.0}}},
            {"dimensions2D", IVec2{.ind{512, 512}}},
            {"ordering", int(ordering)}
        };
        const int SINGLE_VALUE = 0;
        const int SCALAR_MAG = 1;
        Uniforms fragment_uniforms = {
            {"tex1", &m_frames.slices[0]},
            {"drawType1", int(SCALAR_MAG)},
            {"brightness1", params.brightness2
                    / float(pow(2.0, 2.0*params.log2TexWidth))},
            {"color1", Vec4{.r=1.0, .g=0.0, .b=0.0, 
                                .a=params.transparency2}},
            {"tex2", &m_frames.slices[1]},
            {"drawType2", int(SCALAR_MAG)},
            {"brightness2", params.brightness1
                    / float(pow(2.0, 2.0*params.log2TexWidth))},
            {"color2", Vec4{.r=0.0, .g=0.0, .b=1.0,
                            .a=params.transparency1}},
            {"tex3", &m_frames.potential_slice},
            {"drawType3", int(SINGLE_VALUE)},
            {"brightness3", params.potentialBrightness},
            {"color3", Vec4{.r=1.0, .g=1.0, .b=1.0, .a=0.1}},
            {"tex4", &m_frames.potential_slice},
            {"drawType4", int(SINGLE_VALUE)},
            {"brightness4", params.potentialBrightness},
            {"color4", Vec4{.r=1.0, .g=1.0, .b=1.0, .a=0.0}},
        };
        Uniforms uniforms {};
        for (auto &e: vertex_uniforms)
            uniforms.insert(e);
        for (auto &e: fragment_uniforms)
            uniforms.insert(e);
        m_frames.render.draw(
            m_programs.four_surfaces,
            uniforms,
            m_frames.surface,
            Config::viewport(
                0, 0, 
                m_frames.view_params.width, 
                m_frames.view_params.height)
        );
        // uint32_t program = (params.colorPhase)? 
        //     m_programs.surface_domain_color: m_programs.surface_mag_color_map;
        /* uint32_t program = m_programs.surface_mag_color_map;
        Uniforms vertex_uniforms1 = {
            {"heightTex", &m_frames.slices[0]},
            {"rotation", rotation},
            {"screenDimensions", screen_dimensions},
            {"translate", Vec3{.ind{0.0, 0.0, 0.0}}},
            {"heightScale", 
                    params.height2
                    / (float(pow(2.0, 2.0*params.log2TexWidth))
                        * 120)
                },
            {"scale", scale},
            {"dimensions2D", 
                IVec2{.ind{512, 512}}},
            {"heightDataType", int(0)}
        };
        Uniforms vertex_uniforms2 = {
            {"heightTex", &m_frames.slices[1]},
            {"rotation", rotation},
            {"screenDimensions", screen_dimensions},
            {"translate", Vec3{.ind{0.0, 0.0, 0.0}}},
            {"heightScale",
                params.height2
                    / (float(pow(2.0, 2.0*params.log2TexWidth))
                        * 120)
                }, // TODO!
            {"scale", scale},
            {"dimensions2D", 
                IVec2{.ind{512, 512}}},
            {"heightDataType", int(0)}
        };
        Uniforms mag_color_map_uniforms1 {
            {"tex1", &m_frames.slices[0]},
            {"tex2", &m_frames.slices[1]},
            {"brightness",
                    params.brightness2
                    / float(pow(2.0, 2.0*params.log2TexWidth))},
            {"color", Vec3{.r=1.0, .g=0.0, .b=0.0}},
        };
        Uniforms mag_color_map_uniforms2 {
            {"tex1", &m_frames.slices[0]},
            {"tex2", &m_frames.slices[1]},
            {"brightness", 
                    params.brightness1 
                    / float(pow(2.0, 2.0*params.log2TexWidth))},
            {"color", Vec3{.r=0.0, .g=0.0, .b=1.0}},
        };
        for (const auto &e: vertex_uniforms1)
            mag_color_map_uniforms1.insert(e);
        for (const auto &e: vertex_uniforms2)
            mag_color_map_uniforms2.insert(e);
        m_frames.render.draw(
            program,
            mag_color_map_uniforms1,
            m_frames.surface,
            Config::viewport(
                0, 0, 
                m_frames.view_params.width, 
                m_frames.view_params.height)
        );
        m_frames.render.draw(
            program,
            mag_color_map_uniforms2,
            m_frames.surface,
            Config::viewport(
                0, 0, 
                m_frames.view_params.width, 
                m_frames.view_params.height)
        );
        { 
            Uniforms potential_single_color_uniforms {
                {"heightTex", &m_frames.potential_slice},
                {"heightScale", params.potentialHeight/10.0F},
                {"rotation", rotation},
                {"screenDimensions", screen_dimensions},
                {"translate", Vec3{.ind{0.0, 0.0, 0.0}}},
                {"heightScale", params.potentialBrightness}, // TODO!
                {"scale", scale},
                {"dimensions2D", 
                    IVec2{.ind{512, 512}}},
                {"heightDataType", int(0)},
                {"color", Vec4{.r=1.0, .g=1.0, .b=1.0, .a=0.15},
            }
            };
            m_frames.render.draw(
                m_programs.surface_single_color,
                potential_single_color_uniforms,
                m_frames.surface,
                Config::viewport(
                    0, 0, 
                    m_frames.view_params.width, 
                    m_frames.view_params.height)
            );
        }
        m_frames.render_tmp.clear();
        if (hover.has_value() && 
            (params.mouseUsage.selected == 1 
            || params.mouseUsage.selected == 2)) {
            Vec2 location = get_intersection_from_user_input(
                rotation, scale, 
                hover.value());
            IVec4 tex_d_4d = get_texel_dimensions_4d(
                params.log2TexWidth);
            IVec2 slice_coordinates;
            std::cout << "location x: " << location.x << std::endl;
            std::cout << "location y: " << location.y << std::endl;
            if (location.x > 0.0 && location.x < 1.0 && 
                location.y > 0.0 && location.y < 1.0) {
                if (params.mouseUsage.selected == 1)
                    slice_coordinates = {.ind{
                        int(location.x*float(tex_d_4d[0])),
                        int(location.y*float(tex_d_4d[2]))
                    }};
                else
                    slice_coordinates = {.ind{
                        int(location.x*float(tex_d_4d[1])),
                        int(location.y*float(tex_d_4d[3]))
                    }};
                this->wave_func_xy_slice_view(
                    m_frames.render_tmp, slice_coordinates, params);
                Uniforms uniforms = {
                    {"heightTex", &m_frames.render_tmp},
                    {"rotation", rotation},
                    {"screenDimensions", screen_dimensions},
                    {"translate", Vec3{.ind{0.0, 0.0, 0.0}}},
                    {"heightScale", 10.0F*params.brightness3}, // TODO!
                    {"scale", scale},
                    {"dimensions2D", 
                        IVec2{.ind{512, 512}}},
                    {"heightDataType", int(0)},
                    {"tex", &m_frames.render_tmp},
                    {"brightness", params.brightness3}
                };
                m_frames.render.draw(
                    m_programs.surface_domain_color,
                    uniforms,
                    m_frames.surface,
                    Config::viewport(
                        0, 0, 
                        m_frames.view_params.width, 
                        m_frames.view_params.height)
                );
            }
        }*/
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return m_frames.render;
    } else {
        return this->view(params, hover);
    }
}

const RenderTarget &Simulation::view(
    const SimParams &params, const std::optional<Vec2> &hover) {
    // Vec4 d_4d = get_dimensions_4d(params.log2TexWidth);
    IVec4 tex_d_4d = get_texel_dimensions_4d(params.log2TexWidth);
    if (hover.has_value() && 
        (params.mouseUsage.selected == 1 
            || params.mouseUsage.selected == 2)) {
        IVec2 slice_coordinates;
        if (params.mouseUsage.selected == 1)
            slice_coordinates = {.ind{
                int(hover->x*float(tex_d_4d[0])),
                int(hover->y*float(tex_d_4d[2]))
            }};
        else
            slice_coordinates = {.ind{
                int(hover->x*float(tex_d_4d[1])),
                int(hover->y*float(tex_d_4d[3]))
            }};
        this->wave_func_xy_slice_view(
            m_frames.render_tmp, slice_coordinates, params);
    }
    // std::cout << "particle1 prob texture width: " << (this->m_frames.slices[0].width()) << std::endl;
    particle1_prob_view(this->m_frames.slices[0], params);
    particle2_prob_view(this->m_frames.slices[1], params);

    this->m_frames.render.draw(
        m_programs.visualization2,
        {
            // {"tex", &m_frames.sim_tmp},
            {"prob1Tex", &m_frames.slices[0]},
            {"prob2Tex", &m_frames.slices[1]},
            {"potTex", &m_frames.potential_slice},
            {"color1", (params.symmetry == 0)? 
                Vec3{.r=1.0, 0.0, 0.0}: Vec3{.r=1.0, 1.0, 1.0}},
            {"color2", (params.symmetry == 0)? 
                Vec3{.r=0.0, .g=0.0, .b=1.0}: Vec3{.r=1.0, 1.0, 1.0}},
            {"prevExternalVisTex", &m_frames.render_tmp},
            {"waveFunctionBrightness1", 
                    params.brightness1
                     / float(pow(2.0, 2.0*params.log2TexWidth))},
            {"waveFunctionBrightness2", 
                    params.brightness2
                     / float(pow(2.0, 2.0*params.log2TexWidth))},
            {"potentialBrightness", params.potentialBrightness}
        },
        m_frames.quad_wire_frame
    );
    if (hover.has_value()) {
        WireFrame perp_lines
             = get_perp_lines_wire_frame(Vec2{.x=hover->x, .y=hover->y});
        this->m_frames.render.draw(
            m_programs.uniform_color,
            {
                // {"tex", &m_frames.sim_tmp},
                {"color", Vec4{.r=1.0, .g=1.0, .b=1.0, .a=1.0}}
            },
            perp_lines
        );
    }
    if (params.showInitialWavePacketAverages) {
        WireFrame arrow1 = get_arrow_wire_frame(params.pos1, params.momentum1);
        WireFrame arrow2 = get_arrow_wire_frame(params.pos2, params.momentum2);
        WireFrame circle1 = get_circle_wire_frame(params.pos1, 2.5*params.sigma1);
        WireFrame circle2 = get_circle_wire_frame(params.pos2, 2.5*params.sigma2);
        this->m_frames.render.draw(
            m_programs.uniform_color,
            {
                {"color", Vec4{.r=0.0, 0.0, 1.0, 1.0}}
            },
            arrow1
        );
        this->m_frames.render.draw(
            m_programs.uniform_color,
            {
                {"color", Vec4{.r=1.0, 0.0, 0.0, 1.0}}
            },
            arrow2
        );
        this->m_frames.render.draw(
            m_programs.uniform_color,
            {
                {"color", Vec4{.r=0.0, 0.0, 1.0, 1.0}}
            },
            circle1
        );
        this->m_frames.render.draw(
            m_programs.uniform_color,
            {
                {"color", Vec4{.r=1.0, 0.0, 0.0, 1.0}}
            },
            circle2
        );
    }
    return this->m_frames.render;
}