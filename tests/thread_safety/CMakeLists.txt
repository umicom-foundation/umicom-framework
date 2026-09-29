# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
add_executable(umicom-test-thread-safety "${CMAKE_CURRENT_LIST_DIR}/test_lifetime.c")
target_link_libraries(umicom-test-thread-safety PRIVATE Umicom::platform)
umicom_thread_safety_target(umicom-test-thread-safety)
set(_umi_lifetime_cases invalid zero positive negative int_min int_max poll
    release_running destroy_running self_join join_stress release_stress
    condition zero_wait mutex_invalid)
if(UNIX)
    list(APPEND _umi_lifetime_cases native_exit native_cancel signal_sleep tls_completion)
endif()
foreach(_case IN LISTS _umi_lifetime_cases)
    add_test(NAME framework.thread_safety.${_case} COMMAND umicom-test-thread-safety ${_case})
    set_tests_properties(framework.thread_safety.${_case} PROPERTIES TIMEOUT 30
        LABELS "framework;platform;thread_safety")
endforeach()
foreach(_mode IN ITEMS --self-test cancel)
    string(REPLACE "--self-test" "notes" _case "${_mode}")
    add_test(NAME framework.thread_safety.example_${_case} COMMAND umicom-thread-lifecycle ${_mode})
    set_tests_properties(framework.thread_safety.example_${_case} PROPERTIES TIMEOUT 15
        LABELS "framework;platform;thread_safety;learning")
endforeach()
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(umicom-test-thread-safety-faults "${CMAKE_CURRENT_LIST_DIR}/test_faults.c")
    target_link_libraries(umicom-test-thread-safety-faults PRIVATE Umicom::platform)
    target_link_options(umicom-test-thread-safety-faults PRIVATE
        -Wl,--wrap=calloc -Wl,--wrap=free -Wl,--wrap=pthread_create
        -Wl,--wrap=pthread_detach -Wl,--wrap=nanosleep)
    umicom_thread_safety_target(umicom-test-thread-safety-faults)
    foreach(_case IN ITEMS allocation native_create release_failure release_exactly_once
            join_exactly_once sleep_remainder sleep_error)
        add_test(NAME framework.thread_safety.${_case} COMMAND umicom-test-thread-safety-faults ${_case})
        set_tests_properties(framework.thread_safety.${_case} PROPERTIES TIMEOUT 30
            LABELS "framework;platform;thread_safety;fault_injection")
    endforeach()
endif()
if(NOT UMICOM_THREAD_SAFETY_INTEGRATION_CHILD AND CMAKE_GENERATOR MATCHES "Ninja")
    foreach(_mode IN ITEMS immediate deferred minimal)
        add_test(NAME framework.thread_safety.composition_${_mode}
            COMMAND "${CMAKE_COMMAND}"
                "-DFRAMEWORK=${_umi_thread_root}"
                "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/thread-composition-${_mode}"
                "-DMODE=${_mode}" "-DC_COMPILER=${CMAKE_C_COMPILER}"
                "-DCTEST=${CMAKE_CTEST_COMMAND}"
                -P "${CMAKE_CURRENT_LIST_DIR}/check_composition.cmake")
        set_tests_properties(framework.thread_safety.composition_${_mode} PROPERTIES
            TIMEOUT 120 LABELS "framework;thread_safety;integration")
    endforeach()
endif()
