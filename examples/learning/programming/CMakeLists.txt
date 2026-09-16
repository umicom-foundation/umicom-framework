#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/learning/programming/CMakeLists.txt
# PURPOSE: Compile and test executable lessons; never execute unsafe snippets.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomProgrammingLessons LANGUAGES C)
    include(CTest)
endif()
include(GNUInstallDirs)
option(UMICOM_LEARNING_BUILD_ASSEMBLY "Build native x86-64 or RISC-V lesson comparisons" OFF)
option(UMICOM_LEARNING_SANITIZERS "Instrument this lesson project with ASan and UBSan" OFF)

# Explicit targets make the exercised source set reviewable and reproducible.
set(UMICOM_PROGRAMMING_LESSONS
    binary_arithmetic
    boolean_masks
    integer_ranges
    byte_order
    cpu_model
    memory_lifetimes
    types_formatting
    control_flow
    functions_contracts
    array_bounds
    bounded_strings
    numeric_parsing
    records_enums
    pointer_contracts
    dynamic_memory
    checked_arithmetic
    finite_numbers
    resource_cleanup
    modules_build
    callbacks
    controller_architecture
    state_machines
    bounded_queue
    search_sort
    test_design
    coding_conventions
    macro_hazards
    binary_records
    recursion
    atomic_basics
    assembly_branches
    assembly_loops
)
foreach(lesson IN LISTS UMICOM_PROGRAMMING_LESSONS)
    string(REPLACE "_" "-" targetSuffix "${lesson}")
    set(target "umicom-learning-${targetSuffix}")
    add_executable(${target} "${lesson}/main.c")
    if(lesson STREQUAL "modules_build")
        target_sources(${target} PRIVATE "${lesson}/score.c")
    endif()
    set_target_properties(${target} PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE
            "$<$<COMPILE_LANGUAGE:C>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wshadow;-Werror>")
    endif()
    if(UNIX)
        target_link_libraries(${target} PRIVATE m)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
    if(UMICOM_LEARNING_SANITIZERS)
        if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang)$")
            message(FATAL_ERROR "Lesson sanitizers require GCC or Clang.")
        endif()
        target_compile_options(${target} PRIVATE "$<$<COMPILE_LANGUAGE:C>:-fsanitize=address,undefined;-fno-omit-frame-pointer>")
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
    if(BUILD_TESTING)
        add_test(NAME "framework.learning.programming.${lesson}" COMMAND ${target})
        set_tests_properties("framework.learning.programming.${lesson}" PROPERTIES
            LABELS "framework;learning;programming;regression" TIMEOUT 15)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endif()
    install(TARGETS ${target} RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT Learning)
endforeach()

if(UMICOM_LEARNING_BUILD_ASSEMBLY)
    if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang)$" OR APPLE)
        message(FATAL_ERROR "Native lesson Assembly requires GCC/Clang on Windows or an ELF target.")
    endif()
    include(CheckCSourceCompiles)
    check_c_source_compiles("#if !defined(__x86_64__)
#error not x86_64
#endif
int main(void){return 0;}" UMICOM_PROGRAMMING_TARGET_X86_64)
    check_c_source_compiles("#if !defined(__riscv) || __riscv_xlen != 64
#error not riscv64
#endif
int main(void){return 0;}" UMICOM_PROGRAMMING_TARGET_RISCV64)
    if(UMICOM_PROGRAMMING_TARGET_X86_64)
        set(assemblySource basics_x86_64.S)
    elseif(UMICOM_PROGRAMMING_TARGET_RISCV64)
        set(assemblySource basics_riscv64.S)
    else()
        message(FATAL_ERROR "No native Assembly lesson for this CPU; use C-only mode.")
    endif()
    enable_language(ASM)
    foreach(target IN ITEMS umicom-learning-assembly-branches umicom-learning-assembly-loops)
        target_sources(${target} PRIVATE "${assemblySource}")
        target_compile_definitions(${target} PRIVATE UMICOM_LAB_NATIVE_ASSEMBLY=1)
    endforeach()
endif()
