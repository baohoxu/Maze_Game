cmake_minimum_required(VERSION 3.24)
project(GraphicsProject C)

set(CMAKE_C_STANDARD 99)

set(INC_DIR ./include)

include_directories(${INC_DIR})

add_executable(GraphicsProject main.c libgraphics/exceptio.c libgraphics/genlib.c libgraphics/graphics.c libgraphics/linkedlist.c libgraphics/random.c libgraphics/simpio.c libgraphics/strlib.c libgraphics/imgui.c maze.c queue.c file.c)

target_link_libraries(GraphicsProject winmm)
