cmake_minimum_required(VERSION 3.17)
project(FurippuFluid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
option(PROFILE "Profile" OFF)

add_subdirectory(glm)
add_subdirectory(sdl)

add_executable(FurippuFluid main.cpp tracy/TracyClient.cpp)

target_include_directories(FurippuFluid PUBLIC tracy)
target_link_libraries(FurippuFluid PUBLIC pthread SDL2 glm)

if(PROFILE)
    target_compile_definitions(FurippuFluid PUBLIC TRACY_ENABLE)
    target_link_libraries(FurippuFluid PUBLIC dl)
endif()

install(targets FurippuFluid)

set(CPACK_IFW_PACKAGE_TITLE "FurippuFluid")
set(CPACK_IFW_PACKAGE_PUBLISHER "ProtoByter")

include(CPack REQUIRED)
include(CPackIFW REQUIRED)
