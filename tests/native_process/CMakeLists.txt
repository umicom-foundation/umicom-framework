#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/native_process/CMakeLists.txt
# PURPOSE: Native C regression project for tool execution and process ownership.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.21)
project(UmicomNativeProcessChecks LANGUAGES C)
include(CTest)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
option(UMICOM_NATIVE_PROCESS_SANITIZERS "Enable address and undefined-behaviour sanitizers" OFF)
get_filename_component(framework_root "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
find_package(Threads REQUIRED)
add_library(umicom-native-process-check-core STATIC
    "${framework_root}/src/platform/process.c"
    "${framework_root}/src/platform/process_supervisor.c"
    "${framework_root}/src/platform/cancellation.c"
    "${framework_root}/src/platform/threading.c"
    "${framework_root}/src/platform/clock.c")
target_include_directories(umicom-native-process-check-core PUBLIC "${framework_root}/include")
target_link_libraries(umicom-native-process-check-core PUBLIC Threads::Threads)
add_executable(umicom-native-process-fixture process_fixture.c)
add_executable(umicom-native-process-checks test_native_process.c)
target_link_libraries(umicom-native-process-checks PRIVATE umicom-native-process-check-core)
foreach(target umicom-native-process-check-core umicom-native-process-fixture umicom-native-process-checks)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endforeach()
if(WIN32 AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(umicom-native-process-checks PRIVATE -municode)
    target_link_options(umicom-native-process-fixture PRIVATE -municode)
endif()
if(UMICOM_NATIVE_PROCESS_SANITIZERS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Instrument the owner, not the fixture processes intentionally terminated.
    foreach(target umicom-native-process-check-core umicom-native-process-checks)
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endforeach()
endif()
if(BUILD_TESTING)
    foreach(scenario pass nonzero arguments environment duplicate-environment empty-environment
            working-directory unicode-path missing-program missing-directory invalid-request
            invalid-environment invalid-lifetime path-override pre-cancel owned-cancel owned-timeout owned-return owned-failure
            child-compatibility flood-stop raw-output supervisor-stop supervisor-shutdown supervisor-independent
            supervisor-child-compatibility supervisor-invalid-lifetime supervisor-output-tail)
        add_test(NAME "framework.native_process.${scenario}" COMMAND umicom-native-process-checks
            "${scenario}" "$<TARGET_FILE:umicom-native-process-fixture>" "${CMAKE_CURRENT_BINARY_DIR}")
        set_tests_properties("framework.native_process.${scenario}" PROPERTIES TIMEOUT 30)
    endforeach()
    if(WIN32)
        add_test(NAME framework.native_process.invalid-utf8 COMMAND umicom-native-process-checks
            invalid-utf8 "$<TARGET_FILE:umicom-native-process-fixture>" "${CMAKE_CURRENT_BINARY_DIR}")
    endif()
endif()

# The Windows quote encoder is pure C and can be checked on either platform.
add_executable(umicom-native-argument-checks test_windows_arguments.c)
target_include_directories(umicom-native-argument-checks PRIVATE "${framework_root}/include")
add_executable(umicom-native-notes-workflow test_notes_workflow.c)
target_link_libraries(umicom-native-notes-workflow PRIVATE umicom-native-process-check-core)
foreach(target umicom-native-argument-checks umicom-native-notes-workflow)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
        if(UMICOM_NATIVE_PROCESS_SANITIZERS)
            target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=address,undefined)
        endif()
    endif()
endforeach()
if(WIN32 AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(umicom-native-notes-workflow PRIVATE -municode)
endif()
if(BUILD_TESTING)
    foreach(scenario cases bounds)
        add_test(NAME "framework.native_process.windows-arguments-${scenario}"
            COMMAND umicom-native-argument-checks "${scenario}")
    endforeach()
    add_test(NAME framework.native_process.ctest-configure COMMAND "${CMAKE_COMMAND}"
        -S "${CMAKE_CURRENT_SOURCE_DIR}/tree_project" -B "${CMAKE_CURRENT_BINARY_DIR}/ctest-tree" -G Ninja
        "-DUMICOM_PROCESS_FIXTURE=$<TARGET_FILE:umicom-native-process-fixture>")
    set_tests_properties(framework.native_process.ctest-configure PROPERTIES FIXTURES_SETUP native_ctest_tree)
    add_test(NAME framework.native_process.ctest-stop COMMAND umicom-native-process-checks ctest-stop
        "$<TARGET_FILE:umicom-native-process-fixture>" "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CTEST_COMMAND}" "${CMAKE_CURRENT_BINARY_DIR}/ctest-tree")
    set_tests_properties(framework.native_process.ctest-stop PROPERTIES FIXTURES_REQUIRED native_ctest_tree TIMEOUT 30)
    find_program(UMICOM_NOTES_NINJA NAMES ninja REQUIRED)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$" AND
       CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" AND NOT APPLE)
        add_test(NAME framework.native_process.c-assembly-workflow COMMAND umicom-native-notes-workflow
            "${CMAKE_COMMAND}" "${CMAKE_CTEST_COMMAND}"
            "${framework_root}/examples/development/c_assembly_notes" "${CMAKE_CURRENT_BINARY_DIR}"
            "${CMAKE_C_COMPILER}" "${UMICOM_NOTES_NINJA}")
        set_tests_properties(framework.native_process.c-assembly-workflow PROPERTIES TIMEOUT 180)
    endif()
endif()

add_executable(umicom-native-tool-example "${framework_root}/examples/development/native_tool/main.c")
target_link_libraries(umicom-native-tool-example PRIVATE umicom-native-process-check-core)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-native-tool-example PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    if(UMICOM_NATIVE_PROCESS_SANITIZERS)
        target_compile_options(umicom-native-tool-example PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(umicom-native-tool-example PRIVATE -fsanitize=address,undefined)
    endif()
endif()
if(BUILD_TESTING)
    add_test(NAME framework.native_process.supervisor-example COMMAND umicom-native-tool-example)
    set_tests_properties(framework.native_process.supervisor-example PROPERTIES TIMEOUT 20)
endif()

if(BUILD_TESTING AND UNIX)
    add_test(NAME framework.native_process.ignored-sigchld COMMAND umicom-native-process-checks
        ignored-sigchld "$<TARGET_FILE:umicom-native-process-fixture>" "${CMAKE_CURRENT_BINARY_DIR}")
    set_tests_properties(framework.native_process.ignored-sigchld PROPERTIES TIMEOUT 20)
endif()
