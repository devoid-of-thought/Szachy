cmake_minimum_required(VERSION 3.28)
project(Szachy)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define executable and its source files
add_executable(Szachy
        main.cpp
        Plansza.h
        Plansza.cpp
        Figury.h
        Figury.cpp
        Window.cpp
        Window.h
)

# Output binaries in the build folder under Debug/Release
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Add SDL3 and related libraries from source
add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)

# Explicitly add include directories (in case targets don't export them properly)
target_include_directories(Szachy PRIVATE
        ${CMAKE_SOURCE_DIR}/SDL/include
        ${CMAKE_SOURCE_DIR}/SDL_image/include
        ${CMAKE_SOURCE_DIR}/SDL_ttf/include
)

# Link SDL3 libraries to your executable
target_link_libraries(Szachy PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
)
