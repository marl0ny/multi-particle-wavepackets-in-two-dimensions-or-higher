#ifndef _MULTI_SURFACE_
#define _MULTI_SURFACE_

#include "gl_wrappers.hpp"

namespace multi_surface {

    WireFrame get_wireframe(IVec2 d_2d, int number_of_surfaces);

    class MultiSurface {
        std::vector<int> elements;
        std::vector<float> vertices;
        IVec2 d_2d;
        public:
        MultiSurface(IVec2 d_2d, int number_of_surfaces);
        WireFrame get_wire_frame() const;
        void sort_vertices_by_distance_to_point(Vec3 point);
    };
};


#endif