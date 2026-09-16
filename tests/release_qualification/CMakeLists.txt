#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/release_qualification/CMakeLists.txt
#
# PURPOSE:
#   Provide a small, standalone build of the actual naming and learning tests.
#   The normal Framework build already owns these targets and implementations.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomReleaseQualification LANGUAGES C)
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(FATAL_ERROR "Use this small project independently; normal Framework targets already own these tests.")
endif()
enable_testing()
get_filename_component(UMICOM_QUALIFICATION_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
option(UMICOM_QUALIFICATION_SANITIZERS "Instrument the focused tests with ASan and UBSan" OFF)

# Compile the real production source paths, not copies or substitute contracts.
# These libraries are local test subjects. They are never installed into apps.
add_library(umicom-qualification-naming STATIC
    "${UMICOM_QUALIFICATION_ROOT}/src/codeguard/source_naming.c"
    "${UMICOM_QUALIFICATION_ROOT}/src/codeguard/result.c")
add_library(umicom-qualification-learning STATIC
    "${UMICOM_QUALIFICATION_ROOT}/src/teacher/foundations_curriculum.c"
    "${UMICOM_QUALIFICATION_ROOT}/src/teacher/learning_plan.c")
foreach(target IN ITEMS umicom-qualification-naming umicom-qualification-learning)
    target_include_directories(${target} PUBLIC "${UMICOM_QUALIFICATION_ROOT}/include")
endforeach()

add_executable(umicom-codeguard-source-naming-tests
    "${UMICOM_QUALIFICATION_ROOT}/tests/test_codeguard_source_naming.c")
target_link_libraries(umicom-codeguard-source-naming-tests PRIVATE umicom-qualification-naming)
add_test(NAME framework.codeguard_source_naming COMMAND umicom-codeguard-source-naming-tests)
add_executable(umicom-teacher-foundations-curriculum-test
    "${UMICOM_QUALIFICATION_ROOT}/tests/teacher/test_foundations_curriculum.c")
target_link_libraries(umicom-teacher-foundations-curriculum-test PRIVATE umicom-qualification-learning)
add_test(NAME framework.teacher.foundations.curriculum COMMAND umicom-teacher-foundations-curriculum-test)
add_executable(umicom-teacher-foundations-resources-test
    "${UMICOM_QUALIFICATION_ROOT}/tests/teacher/test_foundations_resources.c")
target_link_libraries(umicom-teacher-foundations-resources-test PRIVATE umicom-qualification-learning)
add_test(NAME framework.teacher.foundations.resources
    COMMAND umicom-teacher-foundations-resources-test "${UMICOM_QUALIFICATION_ROOT}")
foreach(check IN ITEMS markup links bounds)
    add_test(NAME "framework.teacher.foundations.resources.${check}"
        COMMAND umicom-teacher-foundations-resources-test --self-test "${check}")
endforeach()
set_tests_properties(framework.codeguard_source_naming
    framework.teacher.foundations.curriculum framework.teacher.foundations.resources
    framework.teacher.foundations.resources.markup framework.teacher.foundations.resources.links
    framework.teacher.foundations.resources.bounds
    PROPERTIES TIMEOUT 30 LABELS "framework;release-qualification;regression")

foreach(target IN ITEMS umicom-qualification-naming umicom-qualification-learning
        umicom-codeguard-source-naming-tests umicom-teacher-foundations-curriculum-test
        umicom-teacher-foundations-resources-test)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    if(UMICOM_QUALIFICATION_SANITIZERS)
        if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
            message(FATAL_ERROR "The sanitizer profile requires GCC or Clang.")
        endif()
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
endforeach()
