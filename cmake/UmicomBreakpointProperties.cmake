# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Shared source properties and complete adapter confirmation remain canonical.
include_guard(GLOBAL)
target_sources(umicom_debug PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/debug/breakpoint_edit.c")
target_sources(umicom_developer PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/breakpoint_sync.c")
if(BUILD_TESTING)
    function(umicom_breakpoint_cases group)
        set(target "umicom-breakpoint-properties-${group}-test")
        add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/breakpoint_edit/test_${group}.c")
        target_link_libraries(${target} PRIVATE Umicom::Framework)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
        foreach(case IN LISTS ARGN)
            add_test(NAME framework.breakpoint_properties.${group}.${case} COMMAND ${target} ${case})
            set_tests_properties(framework.breakpoint_properties.${group}.${case} PROPERTIES
                TIMEOUT 30 LABELS "framework;debugger;breakpoint;regression")
        endforeach()
    endfunction()
    umicom_breakpoint_cases(edit apply clear disable noop remove ownership bounds invalid unrelated changed reuse owner session configuration)
    # Compile both public edit types together and confirm that a registry
    # publication invalidates a retained workspace proposal without altering it.
    umicom_breakpoint_cases(registry upsert remove)
    umicom_breakpoint_cases(reply mapping short extra late-invalid stale changed-request duplicate disabled-request)
    umicom_breakpoint_cases(protocol capabilities disabled request invalid-request missing-verified wrong-verified wrong-array negative-line overflow-line wrong-row excess-count)
    target_sources(umicom-breakpoint-properties-protocol-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c")
    if(TARGET umicom-dap-protocol-fixture)
        add_executable(umicom-breakpoint-properties-platform-test "${CMAKE_CURRENT_LIST_DIR}/../tests/breakpoint_edit/test_platform.c")
        target_link_libraries(umicom-breakpoint-properties-platform-test PRIVATE Umicom::Framework)
        umicom_apply_warnings(umicom-breakpoint-properties-platform-test)
        umicom_apply_sanitizers(umicom-breakpoint-properties-platform-test)
        add_dependencies(umicom-breakpoint-properties-platform-test umicom-dap-protocol-fixture)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-breakpoint-properties-platform-test)
        endif()
        foreach(case success unsupported rejected short malformed)
            add_test(NAME framework.breakpoint_properties.platform.${case} COMMAND umicom-breakpoint-properties-platform-test
                "$<TARGET_FILE:umicom-dap-protocol-fixture>" ${case})
            set_tests_properties(framework.breakpoint_properties.platform.${case} PROPERTIES
                TIMEOUT 30 LABELS "framework;debugger;breakpoint;protocol-fixture;regression")
        endforeach()
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/source-breakpoint-properties.md"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
