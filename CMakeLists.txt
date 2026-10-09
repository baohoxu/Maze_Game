cmake_minimum_required(VERSION 3.24)
project(GraphicsProject C)

set(CMAKE_C_STANDARD 99)

set(INC_DIR ./include
            ./src/core
            ./src/ui
            ./src/common)

include_directories(${INC_DIR})

add_executable(GraphicsProject src/main.c libgraphics/exceptio.c libgraphics/genlib.c libgraphics/graphics.c libgraphics/linkedlist.c libgraphics/random.c libgraphics/simpio.c libgraphics/strlib.c libgraphics/imgui.c src/core/queue.c src/core/file.c src/ui/app.c src/common/para.c)

target_link_libraries(GraphicsProject winmm)
