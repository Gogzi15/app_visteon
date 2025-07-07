cmake_minimum_required(VERSION 3.10)

project(hello)

set(CMAKE_CXX_STANDARD 17)

add_executable(hello src/main.cpp)

add_subdirectory(thirdparty/glfw)

target_link_libraries(hello PRIVATE glfw)

target_include_directories(hello PRIVATE thirdparty/glfw/include)

if(WIN32)
    target_link_libraries(hello PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/pvr/Library/Windows_x86_64/libEGL.lib
    ${CMAKE_SOURCE_DIR}/thirdparty/pvr/Library/Windows_x86_64/libGLESv2.lib
)

elseif(UNIX)
    target_link_libraries(hello PRIVATE GLESv2)
endif()

if(WIN32)
    target_include_directories(hello PRIVATE 
    thirdparty/pvr/include
)
elseif(UNIX)
endif()

target_include_directories(hello PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/tinyGLTF
)

target_compile_definitions(hello PRIVATE

    -DTINYGLTF_IMPLEMENTATION
    -DSTB_IMAGE_IMPLEMENTATION
    -DSTB_IMAGE_WRITE_IMPLEMENTATION
)
