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
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/binary_inspect.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/archive_inspect.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/elf_symbols.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/elf_load_plan.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/memory_audit.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/ipc_trace.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/schedule_trace.c"
    "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/src/education/timer_trace.c"
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

    # Inspect deterministic ELF/PE/COFF fixture headers, a compiler-produced
    # executable and a named file whose path includes spaces. The production
    # parser never executes the inspected file.
    add_executable(umicom-education-binary-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_binary_inspect.c")
    target_link_libraries(umicom-education-binary-test PRIVATE Umicom::education_site)
    if(MSVC)
        target_compile_options(umicom-education-binary-test PRIVATE /W4)
    else()
        target_compile_options(umicom-education-binary-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-binary-test/fixture with spaces")
    add_test(NAME framework.education.binary_inspect
        COMMAND umicom-education-binary-test
            "$<TARGET_FILE:umicom-education-binary-test>"
            "${CMAKE_BINARY_DIR}/education-binary-test/fixture with spaces/sample.obj")
    add_test(NAME framework.education.binary.cli
        COMMAND umicom-education binary --file "$<TARGET_FILE:umicom-education>")
    set_tests_properties(framework.education.binary.cli PROPERTIES
        PASS_REGULAR_EXPRESSION "Format: (ELF|PE)")

    # A real C23 object in a static archive supplements hand-built header
    # fixtures, proving that GCC/Clang ar output uses the same parser.
    add_library(umicom-education-archive-fixture STATIC
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/archive_fixture.c")
    add_executable(umicom-education-archive-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_archive_inspect.c")
    target_link_libraries(umicom-education-archive-test PRIVATE Umicom::education_site)
    foreach(target IN ITEMS umicom-education-archive-fixture umicom-education-archive-test)
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4)
        else()
            target_compile_options(${target} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
    endforeach()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-archive-test/fixture with spaces")
    add_test(NAME framework.education.archive_inspect
        COMMAND umicom-education-archive-test
            "${CMAKE_BINARY_DIR}/education-archive-test/fixture with spaces/lesson sample.a")
    add_test(NAME framework.education.archive.cli
        COMMAND umicom-education archive
            --file "$<TARGET_FILE:umicom-education-archive-fixture>")
    set_tests_properties(framework.education.archive.cli PROPERTIES
        PASS_REGULAR_EXPRESSION "Format: ar")

    # The test itself writes one portable ELF64 fixture under a path with
    # spaces. The CLI case depends on the fixture-producing parser regression,
    # so it is safe under parallel CTest and on non-ELF Windows hosts.
    add_executable(umicom-education-elf-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_elf_symbols.c")
    target_link_libraries(umicom-education-elf-test PRIVATE Umicom::education_site)
    if(MSVC)
        target_compile_options(umicom-education-elf-test PRIVATE /W4)
    else()
        target_compile_options(umicom-education-elf-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-elf-test/fixture with spaces")
    set(_education_elf_fixture
        "${CMAKE_BINARY_DIR}/education-elf-test/fixture with spaces/umicom example.o")
    add_test(NAME framework.education.elf_symbols
        COMMAND umicom-education-elf-test "${_education_elf_fixture}")
    add_test(NAME framework.education.symbols.cli
        COMMAND umicom-education symbols --file "${_education_elf_fixture}")
    set_tests_properties(framework.education.elf_symbols PROPERTIES
        FIXTURES_SETUP education_elf_fixture)
    set_tests_properties(framework.education.symbols.cli PROPERTIES
        FIXTURES_REQUIRED education_elf_fixture
        PASS_REGULAR_EXPRESSION "Format: ELF64")

    # Plan ELF PT_LOAD ranges from a deterministic RV64 fixture, including a
    # filename with spaces. CTest fixture ordering keeps parallel runs safe.
    add_executable(umicom-education-load-plan-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_elf_load_plan.c")
    target_link_libraries(umicom-education-load-plan-test PRIVATE Umicom::education_site)
    if(MSVC)
        target_compile_options(umicom-education-load-plan-test PRIVATE /W4)
    else()
        target_compile_options(umicom-education-load-plan-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-load-plan-test/fixture with spaces")
    set(_education_load_plan_fixture
        "${CMAKE_BINARY_DIR}/education-load-plan-test/fixture with spaces/umicom kernel.elf")
    add_test(NAME framework.education.elf_load_plan
        COMMAND umicom-education-load-plan-test "${_education_load_plan_fixture}")
    add_test(NAME framework.education.load_plan.cli
        COMMAND umicom-education load-plan --file "${_education_load_plan_fixture}")
    set_tests_properties(framework.education.elf_load_plan PROPERTIES
        FIXTURES_SETUP education_load_plan_fixture)
    set_tests_properties(framework.education.load_plan.cli PROPERTIES
        FIXTURES_REQUIRED education_load_plan_fixture
        PASS_REGULAR_EXPRESSION "ELF load plan: ELF64")


    # Share the canonical synthetic ELF fixture. It is generated by the
    # existing load-plan test before either CLI runs, including parallel CTest.
    add_executable(umicom-education-memory-audit-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_memory_audit.c")
    target_link_libraries(umicom-education-memory-audit-test PRIVATE Umicom::education_site)
    if(MSVC)
        target_compile_options(umicom-education-memory-audit-test PRIVATE /W4)
    else()
        target_compile_options(umicom-education-memory-audit-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    add_test(NAME framework.education.memory_audit
        COMMAND umicom-education-memory-audit-test)
    add_test(NAME framework.education.memory_audit.cli
        COMMAND umicom-education memory-audit --file "${_education_load_plan_fixture}")
    set_tests_properties(framework.education.memory_audit.cli PROPERTIES
        FIXTURES_REQUIRED education_load_plan_fixture
        PASS_REGULAR_EXPRESSION "Writable and executable pages: 0")

    # A synthetic bounded trace is created by the parser test before the CLI
    # reads it, including when CTest runs in parallel. The path contains spaces.
    add_executable(umicom-education-ipc-trace-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_ipc_trace.c")
    target_link_libraries(umicom-education-ipc-trace-test PRIVATE Umicom::education_site)
    if(MSVC)
        target_compile_options(umicom-education-ipc-trace-test PRIVATE /W4)
    else()
        target_compile_options(umicom-education-ipc-trace-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-ipc-trace-test/fixture with spaces")
    set(_education_ipc_fixture
        "${CMAKE_BINARY_DIR}/education-ipc-trace-test/fixture with spaces/owned messages.ipc")
    add_test(NAME framework.education.ipc_trace
        COMMAND umicom-education-ipc-trace-test "${_education_ipc_fixture}")
    add_test(NAME framework.education.ipc_trace.cli
        COMMAND umicom-education ipc-trace --file "${_education_ipc_fixture}")
    set_tests_properties(framework.education.ipc_trace PROPERTIES
        FIXTURES_SETUP education_ipc_trace_fixture)
    set_tests_properties(framework.education.ipc_trace.cli PROPERTIES
        FIXTURES_REQUIRED education_ipc_trace_fixture
        PASS_REGULAR_EXPRESSION "IPC trace: 4 records")

    # A classroom schedule trace is a distinct wire format, never a Kernel
    # trace or an IPC capture. The library test creates the immutable fixture
    # before the independent CLI test is allowed to inspect it in parallel.
    add_executable(umicom-education-schedule-trace-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_schedule_trace.c")
    target_link_libraries(umicom-education-schedule-trace-test PRIVATE Umicom::education_site)
    if(MSVC)
        target_compile_options(umicom-education-schedule-trace-test PRIVATE /W4)
    else()
        target_compile_options(umicom-education-schedule-trace-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-schedule-trace-test/fixture with spaces")
    set(_education_schedule_fixture
        "${CMAKE_BINARY_DIR}/education-schedule-trace-test/fixture with spaces/round robin.sch")
    add_test(NAME framework.education.schedule_trace
        COMMAND umicom-education-schedule-trace-test "${_education_schedule_fixture}")
    add_test(NAME framework.education.schedule_trace.cli
        COMMAND umicom-education schedule-trace --file "${_education_schedule_fixture}")
    set_tests_properties(framework.education.schedule_trace PROPERTIES
        FIXTURES_SETUP education_schedule_trace_fixture)
    set_tests_properties(framework.education.schedule_trace.cli PROPERTIES
        FIXTURES_REQUIRED education_schedule_trace_fixture
        PASS_REGULAR_EXPRESSION "Scheduler trace: 11 records")

    # Explicitly monotonic classroom timer/event format. The parser test
    # creates the bounded fixture before the independent CLI case reads it.
    # No Kernel trace or machine interrupt is involved in these tests.
    add_executable(umicom-education-timer-trace-test
        "${UMICOM_EDUCATION_FRAMEWORK_ROOT}/tests/education/test_timer_trace.c")
    target_link_libraries(umicom-education-timer-trace-test PRIVATE Umicom::education_site)
    if(MSVC)
        target_compile_options(umicom-education-timer-trace-test PRIVATE /W4)
    else()
        target_compile_options(umicom-education-timer-trace-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/education-timer-trace-test/fixture with spaces")
    set(_education_timer_fixture
        "${CMAKE_BINARY_DIR}/education-timer-trace-test/fixture with spaces/clock events.tmr")
    add_test(NAME framework.education.timer_trace
        COMMAND umicom-education-timer-trace-test "${_education_timer_fixture}")
    add_test(NAME framework.education.timer_trace.cli
        COMMAND umicom-education timer-trace --file "${_education_timer_fixture}")
    set_tests_properties(framework.education.timer_trace PROPERTIES
        FIXTURES_SETUP education_timer_trace_fixture)
    set_tests_properties(framework.education.timer_trace.cli PROPERTIES
        FIXTURES_REQUIRED education_timer_trace_fixture
        PASS_REGULAR_EXPRESSION "Timer trace: 11 records")

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
        framework.education.binary_inspect framework.education.binary.cli
        framework.education.archive_inspect framework.education.archive.cli
        framework.education.elf_symbols framework.education.symbols.cli
        framework.education.elf_load_plan framework.education.load_plan.cli
        framework.education.memory_audit framework.education.memory_audit.cli
        framework.education.ipc_trace framework.education.ipc_trace.cli
        framework.education.schedule_trace framework.education.schedule_trace.cli
        framework.education.timer_trace framework.education.timer_trace.cli
        PROPERTIES LABELS "education;framework;native;safe-c")
endif()

install(TARGETS umicom-education RUNTIME DESTINATION bin)
