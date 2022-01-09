cmake_minimum_required(VERSION 3.17)
project(FurippuFluid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
option(PROFILE "Profile" OFF)

set(SDL_STATIC OFF)
set(SDL_SHARED ON)

add_subdirectory(glm)
add_subdirectory(sdl)

add_executable(FurippuFluid main.cpp tracy/TracyClient.cpp)

set_property(TARGET FurippuFluid PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(FurippuFluid PUBLIC tracy)
target_link_libraries(FurippuFluid PUBLIC SDL2 SDL2main glm)

if(UNIX)
    target_link_libraries(FurippuFluid PUBLIC pthread)
endif()

if(PROFILE)
    target_compile_definitions(FurippuFluid PUBLIC TRACY_ENABLE)
    target_link_libraries(FurippuFluid PUBLIC dl)
endif()

install(TARGETS FurippuFluid RUNTIME DESTINATION bin COMPONENT app)

set(CPACK_PACKAGE_NAME "FurippyFluid")
set(CPACK_PACKAGE_VENDOR "ProtoByter")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
        "FurippuFluid - A simple, small fluid simulation framework")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "FurippuFluid")

include(InstallRequiredSystemLibraries)
include(CPack REQUIRED)

cpack_add_component(app
        DISPLAY_NAME "FurippuFluid"
        DESCRIPTION "The FurippuFluid Application")
