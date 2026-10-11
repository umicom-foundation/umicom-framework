#-----------------------------------------------------------------------------
# Umicom Framework
# File: tools/umicom-education/CMakeLists.txt
# PURPOSE: Build the Framework-owned Bits2Banking education tool in isolation.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomEducation LANGUAGES C)
include(CTest)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

get_filename_component(UMICOM_EDUCATION_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

add_library(umicom_education_site STATIC
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/site_preview.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/site_content.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/site_renderer.c")
add_library(Umicom::education_site ALIAS umicom_education_site)
target_include_directories(umicom_education_site PUBLIC
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/include")

add_executable(umicom-education main.c)
target_link_libraries(umicom-education PRIVATE Umicom::education_site)

if(WIN32)
    target_link_libraries(umicom_education_site PUBLIC ws2_32)
else()
    # POSIX openat/O_NOFOLLOW provide no-symlink rooted file traversal.
    target_compile_definitions(umicom_education_site PRIVATE _GNU_SOURCE=1)
    target_compile_definitions(umicom-education PRIVATE _GNU_SOURCE=1)
endif()

foreach(target IN ITEMS umicom_education_site umicom-education)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endforeach()

if(BUILD_TESTING)
    add_executable(umicom-education-path-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_site_paths.c")
    target_link_libraries(umicom-education-path-test PRIVATE Umicom::education_site)
    add_test(NAME framework.education.site_paths COMMAND umicom-education-path-test)

    add_executable(umicom-education-publish-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_site_publish.c")
    target_link_libraries(umicom-education-publish-test PRIVATE Umicom::education_site)
    if(NOT WIN32)
        target_compile_definitions(umicom-education-publish-test PRIVATE _GNU_SOURCE=1)
    endif()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-test-root")
    add_test(NAME framework.education.publication
        COMMAND umicom-education-publish-test "${CMAKE_BINARY_DIR}/education-test-root")

    add_test(NAME framework.education.cli.help COMMAND umicom-education --help)
    set_tests_properties(framework.education.site_paths
        framework.education.publication framework.education.cli.help
        PROPERTIES LABELS "education;framework;native;safe-c")
endif()

install(TARGETS umicom-education RUNTIME DESTINATION bin)
