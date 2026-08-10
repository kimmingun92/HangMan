cmake_minimum_required(VERSION 3.10)
project(hangman C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)
find_path(RAYGUI_INCLUDE_DIR raygui.h)

add_executable(hangman
    src/main.c
    src/game.c
    src/words/words.c
)

target_include_directories(hangman PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (RAYGUI_INCLUDE_DIR)
    target_include_directories(hangman PRIVATE ${RAYGUI_INCLUDE_DIR})
    target_compile_definitions(hangman PRIVATE HAVE_RAYGUI=1)
endif()

target_link_libraries(hangman PRIVATE raylib)

if (UNIX AND NOT APPLE)
    target_link_libraries(hangman PRIVATE m pthread dl rt X11)
endif()
