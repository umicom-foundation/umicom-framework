# Umicom Framework - isolated canvas transaction verification.
# Uses the real source files; no replacement production models or test stubs.
cmake_minimum_required(VERSION 3.20)
project(UmicomWorkspaceCanvasTests LANGUAGES C)
enable_testing()

get_filename_component(UMICOM_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang)$" OR APPLE)
    message(FATAL_ERROR "This isolated target requires GCC or GNU-driver Clang with a GNU-compatible linker")
endif()

add_executable(umicom-workspace-canvas-test
    test_canvas_transactions.c
    "${UMICOM_FRAMEWORK_ROOT}/src/ui/workspace_customisation.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/ui/workspace_layout.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/ui/window_group.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/ui/window_catalogue.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/ui/theme_profile.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/ui/brand_palette.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/ui/types.c"
    "${UMICOM_FRAMEWORK_ROOT}/src/ui/workspace_geometry.c"
)
target_include_directories(umicom-workspace-canvas-test PRIVATE
    "${UMICOM_FRAMEWORK_ROOT}/include")
# Link the complete dependency set of these production translation units.
# Do not depend on discarded sections to hide missing Framework symbols:
# ELF and PE/COFF linkers need not retain the same set of functions.
# This verifies the portable workspace subset, not the complete UI target.
target_compile_options(umicom-workspace-canvas-test PRIVATE
    -std=c2x -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)

option(UMICOM_CANVAS_TEST_SANITIZERS "Run isolated tests with address and undefined-behaviour sanitizers" OFF)
option(UMICOM_CANVAS_TEST_ALLOCATION_FAILURES "Inject candidate-allocation failures using the GNU linker" OFF)
if(UMICOM_CANVAS_TEST_SANITIZERS)
    target_compile_options(umicom-workspace-canvas-test PRIVATE
        -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(umicom-workspace-canvas-test PRIVATE -fsanitize=address,undefined)
endif()
if(UMICOM_CANVAS_TEST_ALLOCATION_FAILURES)
    target_compile_definitions(umicom-workspace-canvas-test PRIVATE UMICOM_TEST_WRAP_MALLOC=1)
    target_link_options(umicom-workspace-canvas-test PRIVATE -Wl,--wrap=malloc)
endif()
add_test(NAME framework.workspace_canvas.transactions COMMAND umicom-workspace-canvas-test)
