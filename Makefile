CPP_SOURCES = matrix.cpp multi_surface.cpp surface.cpp parse.cpp \
	user_edit_glsl.cpp reduce4d.cpp simulation.cpp \
	main.cpp \
	interactor.cpp gl_wrappers.cpp glfw_window.cpp
OBJECTS = matrix.o multi_surface.o surface.o parse.o \
	user_edit_glsl.o reduce4d.o simulation.o \
	main.o \
	interactor.o gl_wrappers.o glfw_window.o
SHADERS = ./shaders/visualization/d-color-wfn-gscale-pot.frag ./shaders/visualization/diff-particles-gscale-pot.frag ./shaders/potentials/coulomb-interaction-like.frag ./shaders/util/copy.frag ./shaders/util/scale.frag ./shaders/util/norm-squared.frag ./shaders/util/uniform-color.frag ./shaders/util/zero.frag ./shaders/util/reduce-4x4.frag ./shaders/util/perp-lines.frag ./shaders/util/merge-staggered-complex.frag ./shaders/util/transpose-hypercube-norm-squared.frag ./shaders/util/add2.frag ./shaders/util/add4-r.frag ./shaders/util/rgb-combine.frag ./shaders/util/slice-of-4d.frag ./shaders/surface/four-surfaces.frag ./shaders/surface/single-color.frag ./shaders/surface/mag-color-map.frag ./shaders/surface/surface.vert ./shaders/surface/four-surfaces.vert ./shaders/surface/domain-coloring.frag ./shaders/time-step/leapfrog.frag ./shaders/wavepacket/gaussian.frag

SHELL = /bin/bash
FLAGS = -O0 -g
C_COMPILE = clang
CPP_COMPILE = clang++ -std=c++17
LINKER = /usr/bin/ld

ifeq ($(shell uname),Darwin)
INCLUDE =  -I${PWD} -I${PWD}/gl_wrappers -I/opt/homebrew/include
LIBS = -ldl -L/opt/homebrew/lib -lglfw\
       -framework CoreVideo -framework OpenGL -framework IOKit\
       -framework Cocoa -framework Carbon
else
INCLUDE =  -I${PWD} -I${PWD}/gl_wrappers
LIBS = -lm -lGL -lGLEW -lglfw
endif

# Make sure to source <emcc_location>/emsdk/emsdk_env.sh first!
WEB_TARGET = main.js

TARGET = ${PWD}/program
DATA_DEPENDENCIES = parameters.json
GENERATION_SCRIPTS = make_parameter_files.py
GENERATED_DEPENDENCIES = parameters.hpp
C_SOURCES =
SOURCES = ${C_SOURCES} ${CPP_SOURCES}
# SHADERS = ./shaders/*


all: ${TARGET}

${TARGET}: ${OBJECTS}
	${CPP_COMPILE} ${FLAGS} -o $@ ${OBJECTS} ${LIBS}

${WEB_TARGET}: ${SOURCES} ${GENERATED_DEPENDENCIES} ${SHADERS}
	emcc -lembind -o $@ ${SOURCES} ${INCLUDE} -std=c++17 -O3 -v -s WASM=2 -s USE_GLFW=3 -s MAX_WEBGL_VERSION=2 \
	-s ALLOW_MEMORY_GROWTH=1 -s LLD_REPORT_UNDEFINED --embed-file shaders

${OBJECTS}: ${CPP_SOURCES} ${GENERATED_DEPENDENCIES}
	${CPP_COMPILE} ${FLAGS} -c ${CPP_SOURCES} ${INCLUDE}

${GENERATED_DEPENDENCIES}: ${DATA_DEPENDENCIES} ${GENERATION_SCRIPTS}
	python3 make_parameter_files.py

clean:
	rm -f *.o ${TARGET} *.wasm *.js
