# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Tests are registered once whether reached immediately or through DEFER.
include_guard(GLOBAL)
add_executable(umicom-queue-shutdown-test "${CMAKE_CURRENT_LIST_DIR}/test_shutdown.c")
target_link_libraries(umicom-queue-shutdown-test PRIVATE Umicom::platform)
set_target_properties(umicom-queue-shutdown-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-queue-shutdown-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-queue-shutdown-test)
endif()
set(_queue_cases null thread_running thread_self thread_rejoin empty empty_repeat
    drain cancel_pending cancel_running ignore_cancel escalate post_stop caller_release
    legacy_finish mixed_results worker_self other_queue cancel_all stress competing_join)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND _queue_cases no_result tls_thread tls_queue)
endif()
foreach(_case IN LISTS _queue_cases)
    add_test(NAME framework.queue_shutdown.${_case}
        COMMAND umicom-queue-shutdown-test ${_case})
    set_tests_properties(framework.queue_shutdown.${_case}
        PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77)
endforeach()
if(TARGET umicom-queue-shutdown)
    foreach(_mode IN ITEMS drain cancel)
        add_test(NAME framework.queue_shutdown.notes_${_mode}
            COMMAND umicom-queue-shutdown ${_mode})
        set_tests_properties(framework.queue_shutdown.notes_${_mode}
            PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77)
    endforeach()
endif()
unset(_queue_cases)
unset(_case)
unset(_mode)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(umicom-queue-shutdown-fault-test "${CMAKE_CURRENT_LIST_DIR}/test_faults.c")
    target_link_libraries(umicom-queue-shutdown-fault-test PRIVATE Umicom::platform)
    target_link_options(umicom-queue-shutdown-fault-test PRIVATE
        -Wl,--wrap=UmiThreadTryJoin -Wl,--wrap=UmiThreadRelease -Wl,--wrap=umi_thread_join)
    set_target_properties(umicom-queue-shutdown-fault-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-queue-shutdown-fault-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-queue-shutdown-fault-test)
    endif()
    foreach(_case IN ITEMS join_failure busy_progress release_failure legacy_retry no_blocking_join)
        add_test(NAME framework.queue_shutdown.${_case}
            COMMAND umicom-queue-shutdown-fault-test ${_case})
        set_tests_properties(framework.queue_shutdown.${_case} PROPERTIES TIMEOUT 30)
    endforeach()
endif()

# This test-only executable recompiles canonical sources, not a stand-in thread
# implementation. It proves that unsupported polling never becomes a wait.
get_filename_component(_queue_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_executable(umicom-queue-shutdown-unavailable-test
    "${CMAKE_CURRENT_LIST_DIR}/test_unavailable.c"
    "${_queue_root}/src/platform/threading.c"
    "${_queue_root}/src/platform/cancellation.c"
    "${_queue_root}/src/platform/clock.c"
    "${_queue_root}/src/platform/task.c"
    "${_queue_root}/src/platform/task_queue.c")
find_package(Threads REQUIRED)
target_link_libraries(umicom-queue-shutdown-unavailable-test PRIVATE Threads::Threads)
target_include_directories(umicom-queue-shutdown-unavailable-test PRIVATE "${_queue_root}/include")
target_compile_definitions(umicom-queue-shutdown-unavailable-test PRIVATE UMICOM_THREAD_DISABLE_TRY_JOIN=1)
set_target_properties(umicom-queue-shutdown-unavailable-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-queue-shutdown-unavailable-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-queue-shutdown-unavailable-test)
endif()
add_test(NAME framework.queue_shutdown.unavailable_contract COMMAND umicom-queue-shutdown-unavailable-test)
set_tests_properties(framework.queue_shutdown.unavailable_contract PROPERTIES TIMEOUT 30)
unset(_queue_root)
unset(_case)

if(NOT UMICOM_QUEUE_SHUTDOWN_COMPOSITION_CHILD)
    find_program(_queue_ninja ninja)
    if(_queue_ninja)
        get_filename_component(_queue_framework "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
        foreach(_case IN ITEMS immediate deferred late_sources minimal)
            add_test(NAME framework.queue_shutdown.composition_${_case}
                COMMAND "${CMAKE_COMMAND}"
                "-DFRAMEWORK=${_queue_framework}"
                "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/queue-composition-${_case}"
                "-DMODE=${_case}" "-DCOMPILER=${CMAKE_C_COMPILER}"
                "-DCTEST=${CMAKE_CTEST_COMMAND}" "-DHOST_SYSTEM=${CMAKE_SYSTEM_NAME}"
                -P "${CMAKE_CURRENT_LIST_DIR}/check_composition.cmake")
            set_tests_properties(framework.queue_shutdown.composition_${_case}
                PROPERTIES TIMEOUT 120)
        endforeach()
    endif()
endif()
