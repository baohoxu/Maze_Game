cmake_minimum_required(VERSION 3.24)
project(GraphicsProject C)

set(CMAKE_C_STANDARD 99)

# Include search path: ui -> core -> common, included by plain file name.
set(INC_DIR
        ./include
        ./src/common
        ./src/core
        ./src/ui)

include_directories(${INC_DIR})

# Third-party graphics library (kept as-is).
set(LIBGRAPHICS_SRC
        libgraphics/exceptio.c
        libgraphics/genlib.c
        libgraphics/graphics.c
        libgraphics/linkedlist.c
        libgraphics/random.c
        libgraphics/simpio.c
        libgraphics/strlib.c
        libgraphics/imgui.c)

# core: maze logic and state, no graphics dependency.
set(CORE_SRC
        src/core/param.c
        src/core/queue.c
        src/core/file.c
        src/core/maze_model.c
        src/core/pathfind.c)

# ui: application layer and rendering.
set(UI_SRC
        src/ui/app.c
        src/ui/render.c
        src/ui/menu.c
        src/ui/input.c)

add_executable(GraphicsProject src/main.c ${CORE_SRC} ${UI_SRC} ${LIBGRAPHICS_SRC})

target_link_libraries(GraphicsProject winmm)
