#include "gl_wrappers.hpp"
#include "reduce4d.hpp"

// #include <iostream>

void square_reduce4d::initialize_sum_quads(
    std::vector<Quad> &sum_quads, TextureParams params, 
    int max_dim, int min_dim) {
    for (int n = max_dim; n >= min_dim; n /= 2) {
        params.width = n;
        params.height = n;
        sum_quads.push_back(Quad{params});
    }
}

void square_reduce4d::initialize_sum_quads(
    Quad *sum_quads, TextureParams params, 
    int max_dim, int min_dim) {
    for (int n = max_dim, i = 0; n >= min_dim; n /= 2, i++) {
        params.width = n;
        params.height = n;
        sum_quads[i].reset(params);
    }
}

void square_reduce4d::reduce(std::vector<Quad> &sum_quads,
            uint32_t scale_program, const Quad &src) {
    int i = 0;
    while (sum_quads[i].width() > src.width()/2) 
        i++;
    for (int j = i; j < sum_quads.size(); j++) {
        IVec2 tex_dimensions2d {
            .x=(int)((j == i)? sum_quads[j-1].width(): src.width()),
            .y=(int)((j == i)? sum_quads[j-1].height(): src.height())
        };
        sum_quads[i].draw(
            scale_program,
            {
                {"scale", {4.0F}}, 
                {"texDimensions2D", tex_dimensions2d},
                {"tex", {(j == i)? &src: &sum_quads[j-1]}}
            });
    }
}

Quad *square_reduce4d::reduce(Quad *sum_quads, int min_square_size,
            uint32_t scale_program, const Quad &src) {
    int i = 0;
    // std::cout << "Reduction size: " << size << std::endl;
    while (sum_quads[i].width() > src.width()/2) 
        i++;
    for (int j = i; sum_quads[j].width() >= min_square_size; j++) {
        IVec2 tex_dimensions2d {
            .x=(int)((j == i)? sum_quads[j-1].width(): src.width()),
            .y=(int)((j == i)? sum_quads[j-1].height(): src.height())
        };
        sum_quads[j].draw(
            scale_program,
            {
                {"scale", {4.0F}},
                {"texDimensions2D", tex_dimensions2d},
                {"tex", {(j == i)? &src: &sum_quads[j-1]}}
            });
        if (sum_quads[j].width() == min_square_size)
            return &sum_quads[j];
    }
}