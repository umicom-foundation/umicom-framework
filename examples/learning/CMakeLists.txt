#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/learning/CMakeLists.txt
# PURPOSE: Build the small learning exercises without another Framework copy.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomLearning LANGUAGES C)
    include(CTest)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

option(UMICOM_LEARNING_BUILD_GUI "Build the Framework GTK learning window" OFF)
option(UMICOM_LEARNING_BUILD_ASSEMBLY "Build the architecture-specific Assembly exercise" OFF)
option(UMICOM_LEARNING_USE_INSTALLED_FRAMEWORK "Find an installed Framework SDK" OFF)

if(UMICOM_LEARNING_USE_INSTALLED_FRAMEWORK)
    find_package(UmicomFramework CONFIG REQUIRED)
endif()

# Each target states its own language requirement; these options do not
# silently change the compiler, warnings or flags of other application targets.
function(umicom_learning_target target)
    set_target_properties(${target} PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE
            $<$<COMPILE_LANGUAGE:C>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wshadow>)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()

add_executable(umicom-learning-hello hello/main.c)
add_library(umicom_learning_score STATIC scores/score.c)
target_include_directories(umicom_learning_score PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/scores")
add_executable(umicom-learning-scores scores/main.c)
target_link_libraries(umicom-learning-scores PRIVATE umicom_learning_score)
add_executable(umicom-learning-memory memory/main.c)
add_executable(umicom-learning-bits bits/main.c)
foreach(target IN ITEMS umicom-learning-hello umicom_learning_score
        umicom-learning-scores umicom-learning-memory umicom-learning-bits)
    umicom_learning_target(${target})
endforeach()

if(BUILD_TESTING)
    add_executable(umicom-learning-score-test scores/test_score.c)
    target_link_libraries(umicom-learning-score-test PRIVATE umicom_learning_score)
    umicom_learning_target(umicom-learning-score-test)
    add_test(NAME framework.learning.score COMMAND umicom-learning-score-test)
    add_test(NAME framework.learning.hello COMMAND umicom-learning-hello)
    set_tests_properties(framework.learning.hello PROPERTIES
        PASS_REGULAR_EXPRESSION "Hello from Umicom[.]")
    add_test(NAME framework.learning.decisions COMMAND umicom-learning-scores)
    add_test(NAME framework.learning.memory COMMAND umicom-learning-memory)
    add_test(NAME framework.learning.bits COMMAND umicom-learning-bits)
    set_tests_properties(framework.learning.bits PROPERTIES PASS_REGULAR_EXPRESSION "00101101")
    set_tests_properties(framework.learning.score framework.learning.hello
        framework.learning.decisions framework.learning.memory framework.learning.bits
        PROPERTIES LABELS "framework;learning;native")
endif()

if(UMICOM_LEARNING_BUILD_ASSEMBLY)
    if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang)$" OR APPLE)
        message(FATAL_ERROR "The Assembly exercise needs GCC/Clang on Windows or an ELF target.")
    endif()
    include(CheckCSourceCompiles)
    check_c_source_compiles("#if !defined(__x86_64__)
#error not x86_64
#endif
int main(void){return 0;}"
        UMICOM_LEARNING_TARGET_X86_64)
    check_c_source_compiles("#if !defined(__riscv) || __riscv_xlen != 64
#error not riscv64
#endif
int main(void){return 0;}"
        UMICOM_LEARNING_TARGET_RISCV64)
    if(UMICOM_LEARNING_TARGET_X86_64)
        set(assembly_source assembly/add_x86_64.S)
    elseif(UMICOM_LEARNING_TARGET_RISCV64)
        set(assembly_source assembly/add_riscv64.S)
    else()
        message(FATAL_ERROR "No Assembly exercise for this target. Build the C lessons with UMICOM_LEARNING_BUILD_ASSEMBLY=OFF.")
    endif()
    enable_language(ASM)
    add_executable(umicom-learning-assembly assembly/main.c assembly/add.c ${assembly_source})
    umicom_learning_target(umicom-learning-assembly)
    if(BUILD_TESTING)
        add_test(NAME framework.learning.assembly COMMAND umicom-learning-assembly)
        set_tests_properties(framework.learning.assembly PROPERTIES LABELS "framework;learning;assembly")
    endif()
endif()

if(UMICOM_LEARNING_BUILD_GUI)
    # In the superbuild this target may be declared later in the same configure.
    # The generator resolves it, and fails rather than substituting raw GTK.
    add_executable(umicom-learning-window window/main.c)
    target_link_libraries(umicom-learning-window PRIVATE Umicom::ui_gtk4)
    umicom_learning_target(umicom-learning-window)
    install(TARGETS umicom-learning-window RUNTIME DESTINATION bin COMPONENT Learning)
endif()

install(TARGETS umicom-learning-hello umicom-learning-scores
    umicom-learning-memory umicom-learning-bits RUNTIME DESTINATION bin COMPONENT Learning)
if(TARGET umicom-learning-assembly)
    install(TARGETS umicom-learning-assembly RUNTIME DESTINATION bin COMPONENT Learning)
endif()

# The extended programming course uses the same source/install ownership.
add_subdirectory(programming)
