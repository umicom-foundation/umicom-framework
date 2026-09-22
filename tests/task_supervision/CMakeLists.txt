#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/task_supervision/CMakeLists.txt
# PURPOSE: Independently build task ownership and cooperative Stop regressions.
# AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomTaskSupervisionChecks LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
find_package(Threads REQUIRED)
option(UMICOM_TASK_CHECKS_SANITIZERS "Enable ASan and UBSan" OFF)
option(UMICOM_TASK_CHECKS_THREAD_SANITIZER "Enable ThreadSanitizer separately" OFF)
if(UMICOM_TASK_CHECKS_SANITIZERS AND UMICOM_TASK_CHECKS_THREAD_SANITIZER)
    message(FATAL_ERROR "Use separate build directories for ASan and TSan")
endif()
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    if(UMICOM_TASK_CHECKS_SANITIZERS)
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    elseif(UMICOM_TASK_CHECKS_THREAD_SANITIZER)
        add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
        add_link_options(-fsanitize=thread)
    endif()
endif()
get_filename_component(framework_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(task_sources
    "${framework_root}/src/platform/task.c"
    "${framework_root}/src/platform/task_queue.c"
    "${framework_root}/src/platform/threading.c"
    "${framework_root}/src/platform/clock.c"
    "${framework_root}/src/platform/cancellation.c")
add_library(umicom-supervision-checks-core STATIC ${task_sources})
target_include_directories(umicom-supervision-checks-core PUBLIC "${framework_root}/include")
target_link_libraries(umicom-supervision-checks-core PUBLIC Threads::Threads)
add_executable(umicom-task-supervision-checks test_tasks.c)
target_link_libraries(umicom-task-supervision-checks PRIVATE umicom-supervision-checks-core)
if(BUILD_TESTING)
    foreach(scenario success failure returned-cancel cancel-created wait-timeout
            cancel-queued-release release-queued-cancel-all cancel-running release-running
            cooperation-required full-queue-retry shutdown-drain shutdown-pending
            shutdown-running shutdown-escalate cancel-all-reusable
            worker-join-refused worker-wait-refused worker-request-shutdown
            progress progress-cancel many-tasks cancel-completion-races
            retained-concurrent double-submit invalid)
        add_test(NAME "framework.task_supervision.${scenario}"
            COMMAND umicom-task-supervision-checks "${scenario}")
        set_tests_properties("framework.task_supervision.${scenario}" PROPERTIES TIMEOUT 30)
    endforeach()
endif()

# Execute the existing native process implementation; no shell-command adapter
# or fake process is substituted for these cancellation/timeout scenarios.
add_executable(umicom-supervision-process-helper process_helper.c)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" AND
        (UMICOM_TASK_CHECKS_SANITIZERS OR UMICOM_TASK_CHECKS_THREAD_SANITIZER))
    target_compile_options(umicom-supervision-process-helper PRIVATE -fno-sanitize=all)
    target_link_options(umicom-supervision-process-helper PRIVATE -fno-sanitize=all)
endif()
add_executable(umicom-process-task-checks test_process_task.c
    "${framework_root}/src/platform/process.c")
target_link_libraries(umicom-process-task-checks PRIVATE umicom-supervision-checks-core)
if(BUILD_TESTING)
    foreach(scenario cancel queue-shutdown timeout success failure)
        add_test(NAME "framework.task_supervision.process.${scenario}"
            COMMAND umicom-process-task-checks
                "$<TARGET_FILE:umicom-supervision-process-helper>" "${scenario}")
        set_tests_properties("framework.task_supervision.process.${scenario}" PROPERTIES TIMEOUT 30)
    endforeach()
endif()

set(ctest_metadata_sources
    "${framework_root}/src/test_platform/ctest.c"
    "${framework_root}/src/test_platform/item.c"
    "${framework_root}/src/test_platform/suite.c"
    "${framework_root}/src/test_platform/discovery.c")
add_library(umicom-background-discovery-core STATIC ${ctest_metadata_sources})
target_include_directories(umicom-background-discovery-core PUBLIC "${framework_root}/include")
target_link_libraries(umicom-background-discovery-core PUBLIC umicom-supervision-checks-core)
add_executable(umicom-background-discovery-checks test_background.c)
target_link_libraries(umicom-background-discovery-checks PRIVATE umicom-background-discovery-core)
add_executable(umicom-background-discovery-native-checks test_background_native.c
    "${framework_root}/src/platform/process.c")
target_link_libraries(umicom-background-discovery-native-checks PRIVATE umicom-background-discovery-core)
if(BUILD_TESTING)
    set(injected_root "${CMAKE_CURRENT_BINARY_DIR}/injected discovery")
    file(MAKE_DIRECTORY "${injected_root}")
    file(WRITE "${injected_root}/CTestTestfile.cmake" "# Injected process tests only.\n")
    foreach(scenario success malformed process-failure stale-generation stale-items
            stale-suites stale-discoveries cancel-running cancel-completed
            cancel-queued-destroy wrong-thread queue-rejected invalid)
        add_test(NAME "framework.task_supervision.discovery.${scenario}"
            COMMAND umicom-background-discovery-checks "${scenario}" "${injected_root}")
        set_tests_properties("framework.task_supervision.discovery.${scenario}" PROPERTIES TIMEOUT 30)
    endforeach()
    set(large_root "${CMAKE_CURRENT_BINARY_DIR}/large discovery $<CONFIG>")
    add_test(NAME framework.task_supervision.discovery.native-setup
        COMMAND "${CMAKE_COMMAND}" -S "${framework_root}/tests/ctest_execution/json_fixture"
            -B "${large_root}" -DCASE_COUNT=10000)
    set_tests_properties(framework.task_supervision.discovery.native-setup PROPERTIES
        FIXTURES_SETUP supervision_large TIMEOUT 120)
    add_test(NAME framework.task_supervision.discovery.native-large
        COMMAND umicom-background-discovery-native-checks "${large_root}")
    set_tests_properties(framework.task_supervision.discovery.native-large PROPERTIES
        FIXTURES_REQUIRED supervision_large TIMEOUT 120)
endif()
