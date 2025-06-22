cmake_minimum_required(VERSION 3.10)
project(my_sdl_app C)

# Find SDL2
find_package(SDL2 REQUIRED)

# Include headers from inc/
include_directories(inc ${SDL2_INCLUDE_DIRS})

# Add source files from src/
file(GLOB SOURCES "src/*.c")

# Define the executable
add_executable(my_sdl_app ${SOURCES})

# Link SDL2
target_link_libraries(my_sdl_app PRIVATE ${SDL2_LIBRARIES})
