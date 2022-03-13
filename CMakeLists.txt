cmake_minimum_required(VERSION 3.10)

project(OpenGLApp)

add_executable(OpenGLApp src/Main.cpp)

add_definitions(-DGLEW_STATIC)

target_include_directories(
    OpenGLApp
    PRIVATE
    dependencies/GLFW/include
    dependencies/GLEW/include
)

target_link_libraries(OpenGLApp ${CMAKE_SOURCE_DIR}/dependencies/GLFW/lib-vc2022/glfw3.lib)
target_link_libraries(OpenGLApp ${CMAKE_SOURCE_DIR}/dependencies/GLEW/lib/Release/x64/glew32s.lib)
target_link_libraries(OpenGLApp opengl32.lib)

set(CMAKE_EXE_LINKER_FLAGS "/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT")