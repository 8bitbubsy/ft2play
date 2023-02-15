cmake_minimum_required(VERSION 3.7)

project(ft2play C)

find_package(SDL2 REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ft2play_SOURCE_DIR}/ft2play/release/other/")

file(GLOB ft2play_SRC
    "${ft2play_SOURCE_DIR}/audiodrivers/sdl/*.c"
    "${ft2play_SOURCE_DIR}/*.c"
    "${ft2play_SOURCE_DIR}/ft2play/src/*.c"
)

add_executable(ft2play ${ft2play_SRC})

target_include_directories(ft2play SYSTEM
    PRIVATE ${SDL2_INCLUDE_DIRS})

if("${SDL2_LIBRARIES}" STREQUAL "")
    message(WARNING "SDL2_LIBRARIES wasn't set, manually setting to SDL2::SDL2")
    set(SDL2_LIBRARIES "SDL2::SDL2")
endif()

target_link_libraries(ft2play
    PRIVATE m pthread ${SDL2_LIBRARIES})

target_compile_definitions(ft2play
    PRIVATE AUDIODRIVER_SDL)

install(TARGETS ft2play
    RUNTIME DESTINATION bin)
