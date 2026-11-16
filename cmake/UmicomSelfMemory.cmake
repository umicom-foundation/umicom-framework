# Umicom Framework
# File: cmake/UmicomSelfMemory.cmake
#
# PURPOSE:
#   Compose native self-process observations and bounded comparison regressions in the existing desktop SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
get_filename_component(_self_memory_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_desktop_system PRIVATE
    "${_self_memory_root}/src/desktop_system/self_memory.c"
    "${_self_memory_root}/src/desktop_system/self_memory_native.c"
    "${_self_memory_root}/src/desktop_system/self_memory_internal.h")
if(WIN32)
    # Psapi version 1 resolves through this system import library; version 2
    # maps to Kernel32. No downloadable runtime or extra process rights.
    target_link_libraries(umicom_desktop_system PRIVATE psapi)
endif()
if(BUILD_TESTING)
    add_executable(umicom-self-memory-test "${_self_memory_root}/tests/desktop_system/test_self_memory.c")
    add_executable(umicom-self-memory-host-test "${_self_memory_root}/tests/desktop_system/test_self_memory_host.c")
    add_executable(umicom-self-memory-example-test "${_self_memory_root}/examples/self_memory/main.c")
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    umicom_apply_application_branding(TARGET umicom-self-memory-example-test
        PRODUCT_NAME "Umicom Memory Observation" INTERNAL_NAME "umicom-memory-observation"
        APPLICATION_ID "org.umicom.sdk.memory.observation" NO_RUNTIME_DEPLOYMENT)
    set(_self_memory_targets umicom-self-memory-test umicom-self-memory-host-test umicom-self-memory-example-test)
    if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_executable(umicom-self-memory-native-test "${_self_memory_root}/tests/desktop_system/test_self_memory_native.c")
        list(APPEND _self_memory_targets umicom-self-memory-native-test)
        if(WIN32)
            set(_self_native_cases valid identity-failure memory-failure)
        else()
            set(_self_native_cases valid identity-failure unknown-resident peak-failure)
        endif()
        foreach(_case IN LISTS _self_native_cases)
            add_test(NAME "framework.desktop_system.self_memory_native.${_case}" COMMAND umicom-self-memory-native-test "${_case}")
            set_tests_properties("framework.desktop_system.self_memory_native.${_case}" PROPERTIES TIMEOUT 20 LABELS "framework;desktop-system;memory;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("framework.desktop_system.self_memory_native.${_case}" TARGETS umicom-self-memory-native-test)
            endif()
        endforeach()
    endif()
    foreach(_target IN LISTS _self_memory_targets)
        target_link_libraries(${_target} PRIVATE Umicom::desktop_system)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_self_memory_root}/include/umicom/desktop_system/self_memory.h"
                "${_self_memory_root}/src/desktop_system/self_memory.c"
                "${_self_memory_root}/src/desktop_system/self_memory_native.c"
                "${_self_memory_root}/src/desktop_system/self_memory_internal.h")
        endif()
    endforeach()
    foreach(_case IN ITEMS capture
            capture-error
            capture-invalid
            capture-null
            growth
            shrink
            equal
            unsigned-range
            partial
            no-common
            pid-change
            creation-change
            provider-change
            time-reversed
            peak-reversed
            null-before
            null-after
            null-output
            zero-pid
            unknown-provider
            unknown-bits
            empty-metrics
            unknown-resident-value
            unknown-peak-value
            unknown-private-value
            zero-known-values)
        add_test(NAME "framework.desktop_system.self_memory.${_case}" COMMAND umicom-self-memory-test "${_case}")
        set_tests_properties("framework.desktop_system.self_memory.${_case}" PROPERTIES TIMEOUT 20 LABELS "framework;desktop-system;memory;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.desktop_system.self_memory.${_case}" TARGETS umicom-self-memory-test)
        endif()
    endforeach()
    if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_test(NAME framework.desktop_system.self_memory_example COMMAND umicom-self-memory-example-test)
        set_tests_properties(framework.desktop_system.self_memory_example PROPERTIES TIMEOUT 20 LABELS "framework;desktop-system;memory;example")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.desktop_system.self_memory_example TARGETS umicom-self-memory-example-test)
        endif()
    endif()
    foreach(_case IN ITEMS capture repeated)
        add_test(NAME "framework.desktop_system.self_memory_host.${_case}" COMMAND umicom-self-memory-host-test "${_case}")
        set_tests_properties("framework.desktop_system.self_memory_host.${_case}" PROPERTIES TIMEOUT 30 LABELS "framework;desktop-system;memory;native")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.desktop_system.self_memory_host.${_case}" TARGETS umicom-self-memory-host-test)
        endif()
    endforeach()
endif()
foreach(_component IN ITEMS Framework FrameworkSDK)
    install(DIRECTORY "${_self_memory_root}/examples/self_memory/"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/self_memory"
        COMPONENT ${_component})
endforeach()
unset(_self_memory_root)
unset(_self_memory_targets)
unset(_self_native_cases)

# Shared serialization keeps application output policy separate from native counters.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSelfMemoryReports.cmake")
