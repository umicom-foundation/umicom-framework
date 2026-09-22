#-----------------------------------------------------------------------------
# Umicom Framework | Sammy Hegab | Umicom Foundation | MIT
# File: tests/ctest_jobs/CMakeLists.txt
# Build the real queued CTest adapter and its previous regression families.
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomCtestJobChecks LANGUAGES C)
include(CTest)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
option(UMICOM_CTEST_JOBS_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)
option(UMICOM_CTEST_JOBS_THREAD_SANITIZER "Enable ThreadSanitizer" OFF)
if(UMICOM_CTEST_JOBS_SANITIZERS AND UMICOM_CTEST_JOBS_THREAD_SANITIZER)
    message(FATAL_ERROR "Select ASan/UBSan or ThreadSanitizer in separate directories")
endif()
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    if(UMICOM_CTEST_JOBS_SANITIZERS)
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    elseif(UMICOM_CTEST_JOBS_THREAD_SANITIZER)
        add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
        add_link_options(-fsanitize=thread)
    endif()
endif()
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../ctest_execution" "adapter")
add_executable(umicom-ctest-job-fixture fixture_helper.c)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" AND
        (UMICOM_CTEST_JOBS_SANITIZERS OR UMICOM_CTEST_JOBS_THREAD_SANITIZER))
    target_compile_options(umicom-ctest-job-fixture PRIVATE -fno-sanitize=all)
    target_link_options(umicom-ctest-job-fixture PRIVATE -fno-sanitize=all)
endif()
add_executable(umicom-ctest-job-checks test_jobs.c)
target_link_libraries(umicom-ctest-job-checks PRIVATE umicom-ctest-json-core
    umicom-ctest-checks-core umicom-ctest-checks-process)
get_filename_component(framework_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_executable(umicom-ctest-job-example "${framework_root}/examples/testing/queued_ctest.c")
target_link_libraries(umicom-ctest-job-example PRIVATE umicom-ctest-checks-core umicom-ctest-checks-process)
if(BUILD_TESTING)
    foreach(root A B)
        set(fixture_${root} "${CMAKE_CURRENT_BINARY_DIR}/Notes ${root} $<CONFIG>")
        add_test(NAME framework.ctest_jobs.setup-${root}
            COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_CURRENT_LIST_DIR}/fixture"
            -B "${fixture_${root}}" "-DHELPER=$<TARGET_FILE:umicom-ctest-job-fixture>"
            "-DROOT_LABEL=${root}")
        set_tests_properties(framework.ctest_jobs.setup-${root} PROPERTIES
            FIXTURES_SETUP "ctest_jobs_${root}" TIMEOUT 60)
    endforeach()
    foreach(case invalid bounds strings duplicate invalid-name created
            success literal repeat continue-failure stop-failure disabled skip
            process-timeout test-timeout missing-build missing-name config
            copied-request two-roots cancel-created cancel-queued cancel-active
            late-cancel partial-progress unrelated-task full-queue double-submit
            wrong-thread large-output provenance provenance-missing
            provenance-mismatch provenance-bounds)
        add_test(NAME "framework.ctest_jobs.${case}"
            COMMAND umicom-ctest-job-checks "${case}" "${fixture_A}" "${fixture_B}")
        set_tests_properties("framework.ctest_jobs.${case}" PROPERTIES
            FIXTURES_REQUIRED "ctest_jobs_A;ctest_jobs_B"
            RESOURCE_LOCK ctest_jobs_fixture TIMEOUT 30)
    endforeach()
endif()
