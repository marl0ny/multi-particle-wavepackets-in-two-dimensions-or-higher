#include "multi_surface.hpp"

struct Vertex {
    float surface_ind;
    float x, y;
    float sort_val;
};

struct Element {
    int v1, v2, v3;
};

WireFrame multi_surface::get_wireframe(IVec2 d_2d, int number_of_surfaces) {
    int width = d_2d[0];
    int height = d_2d[1];
    std::vector <float> vertices {};
    std::vector <int> elements {};
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < number_of_surfaces; k++) {
                Vertex vertex {
                    .surface_ind=float(k),
                    .x=((float)j + 0.5F)/float(width),
                    .y=((float)i + 0.5F)/float(height),
                    .sort_val=((float)j + 0.5F)/float(height)
                };
                vertices.push_back(vertex.surface_ind);
                vertices.push_back(vertex.x);
                vertices.push_back(vertex.y);
                vertices.push_back(vertex.sort_val);
                if (i < (width - 1) && j < (height - 1)) {
                    Element triangle1 {
                        .v1=number_of_surfaces*(j + i*width) + k,
                        .v2=number_of_surfaces*((j + 1) + i*width) + k,
                        .v3=number_of_surfaces*((j + 1) + (i + 1)*width) + k
                    };
                    Element triangle2 {
                        .v1=number_of_surfaces*((j + 1) + (i + 1)*width) + k,
                        .v2=number_of_surfaces*(j + (i + 1)*width) + k,
                        .v3=number_of_surfaces*(j + i*width) + k,
                    };
                    elements.push_back(triangle1.v1);
                    elements.push_back(triangle1.v2);
                    elements.push_back(triangle1.v3);
                    elements.push_back(triangle2.v1);
                    elements.push_back(triangle2.v2);
                    elements.push_back(triangle2.v3);
                }
            }
        }
    }
    Attributes attributes = {
        {"position", {
            .size=4, .type=GL_FLOAT, .normalized=false, .stride=0, .offset=0
        }}
    };
    return WireFrame(attributes, vertices, elements, WireFrame::TRIANGLES);
}

multi_surface::
MultiSurface::
MultiSurface
(IVec2 d_2d, int number_of_surfaces): d_2d(d_2d) {
    int width = d_2d[0];
    int height = d_2d[1];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < number_of_surfaces; k++) {
                Vertex vertex {
                    .surface_ind=float(k),
                    .x=((float)j + 0.5F)/float(width),
                    .y=((float)i + 0.5F)/float(height),
                    .sort_val=((float)j + 0.5F)/float(height)
                };
                this->vertices.push_back(vertex.surface_ind);
                this->vertices.push_back(vertex.x);
                this->vertices.push_back(vertex.y);
                this->vertices.push_back(vertex.sort_val);
                if (i < (width - 1) && j < (height - 1)) {
                    Element triangle1 {
                        .v1=number_of_surfaces*(j + i*width) + k,
                        .v2=number_of_surfaces*((j + 1) + i*width) + k,
                        .v3=number_of_surfaces*((j + 1) + (i + 1)*width) + k
                    };
                    Element triangle2 {
                        .v1=number_of_surfaces*((j + 1) + (i + 1)*width) + k,
                        .v2=number_of_surfaces*(j + (i + 1)*width) + k,
                        .v3=number_of_surfaces*(j + i*width) + k,
                    };
                    this->elements.push_back(triangle1.v1);
                    this->elements.push_back(triangle1.v2);
                    this->elements.push_back(triangle1.v3);
                    this->elements.push_back(triangle2.v1);
                    this->elements.push_back(triangle2.v2);
                    this->elements.push_back(triangle2.v3);
                }
            }
        }
    }
}

WireFrame multi_surface::MultiSurface::get_wire_frame() const {
    Attributes attributes = {
        {"position", {
            .size=4, .type=GL_FLOAT, .normalized=false, .stride=0, .offset=0
        }}
    };
    return WireFrame(attributes, vertices, elements, WireFrame::TRIANGLES);
}
