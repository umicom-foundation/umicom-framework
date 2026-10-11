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
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/site_renderer.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/book.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/publication.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/nav_audit.c"
    # Keep hashing with the existing Framework base implementation. This
    # standalone learning build deliberately does not duplicate its algorithm.
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/base/sha256.c")
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

    add_executable(umicom-education-book-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_book.c")
    target_link_libraries(umicom-education-book-test PRIVATE Umicom::education_site)
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-book-test")
    add_test(NAME framework.education.book
        COMMAND umicom-education-book-test "${CMAKE_BINARY_DIR}/education-book-test")

    add_executable(umicom-education-test-python
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/mock_python.c")
    add_executable(umicom-education-publication-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_publication.c")
    target_link_libraries(umicom-education-publication-test PRIVATE Umicom::education_site)
    if(NOT WIN32)
        target_compile_definitions(umicom-education-publication-test PRIVATE _GNU_SOURCE=1)
    endif()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-publication-test")
    add_test(NAME framework.education.publication_manifest
        COMMAND umicom-education-publication-test
                "${CMAKE_BINARY_DIR}/education-publication-test")

    add_executable(umicom-education-renderer-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_renderer.c")
    target_link_libraries(umicom-education-renderer-test PRIVATE Umicom::education_site)
    if(NOT WIN32)
        target_compile_definitions(umicom-education-renderer-test PRIVATE _GNU_SOURCE=1)
    endif()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-renderer-test")
    add_test(NAME framework.education.renderer
        COMMAND umicom-education-renderer-test
          "${CMAKE_BINARY_DIR}/education-renderer-test"
          "$<TARGET_FILE:umicom-education-test-python>")

    foreach(test_target IN ITEMS umicom-education-book-test
             umicom-education-test-python umicom-education-renderer-test
             umicom-education-publication-test)
        if(MSVC)
            target_compile_options(${test_target} PRIVATE /W4)
        else()
            target_compile_options(${test_target} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endforeach()

    add_executable(umicom-education-navigation-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_nav_audit.c")
    target_link_libraries(umicom-education-navigation-test PRIVATE Umicom::education_site)
    if(MSVC)
        target_compile_options(umicom-education-navigation-test PRIVATE /W4)
    else()
        target_compile_options(umicom-education-navigation-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-navigation-test")
    add_test(NAME framework.education.navigation_audit
        COMMAND umicom-education-navigation-test
            "${CMAKE_BINARY_DIR}/education-navigation-test")

    add_test(NAME framework.education.cli.help COMMAND umicom-education --help)

    # Real native CLI preflight: refuse to serve a missing site and propagate
    # an explicitly requested renderer failure without opening a socket.
    # CMake's execute_process preserves paths with spaces on Windows and Linux.
    add_test(NAME framework.education.serve.preflight
        COMMAND "${CMAKE_COMMAND}"
            "-DEDUCATION_PROGRAM=$<TARGET_FILE:umicom-education>"
            "-DEDUCATION_FIXTURE_ROOT=${CMAKE_BINARY_DIR}/education-serve-preflight"
            -P "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_serve_preflight.cmake")
    set_tests_properties(framework.education.site_paths
        framework.education.publication framework.education.cli.help
        framework.education.book framework.education.renderer
        framework.education.publication_manifest framework.education.serve.preflight
        framework.education.navigation_audit
        PROPERTIES LABELS "education;framework;native;safe-c")
endif()

install(TARGETS umicom-education RUNTIME DESTINATION bin)
