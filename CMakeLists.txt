cmake_minimum_required(VERSION 2.4)
project(PathTracer)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 14)

set(PROJECT_CODE 
    src/vector.cpp include/vector.hpp
    src/utils.cpp include/utils.hpp
    include/core.hpp
    src/texture.cpp include/texture.hpp
    src/environmentMap.cpp include/environmentMap.hpp
    src/brdf.cpp include/brdf.hpp
    src/lambertBrdf.cpp include/lambertBrdf.hpp
    include/baseMaterial.hpp
    src/solidMaterial.cpp include/solidMaterial.hpp
    src/texturedMaterial.cpp include/texturedMaterial.hpp
    src/object.cpp include/object.hpp
    src/sphere.cpp include/sphere.hpp
    src/plane.cpp include/plane.hpp
    src/rectangle.cpp include/rectangle.hpp
    src/ellipse.cpp include/ellipse.hpp
    include/light.hpp
    src/directionalLight.cpp include/directionalLight.hpp
    src/pointLight.cpp include/pointLight.hpp
    src/scene.cpp include/scene.hpp
    src/camera.cpp include/camera.hpp
    src/renderer.cpp include/renderer.hpp
    src/main.cpp)

include_directories(include)

add_executable(PathTracer ${PROJECT_CODE})
