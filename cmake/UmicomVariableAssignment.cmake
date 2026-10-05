# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Public implementation is owned by platform.c and variable_reply.c. Tests link
# the real library and use a deterministic peer that never debugs user programs.
include_guard(GLOBAL)
if(BUILD_TESTING)
    function(umicom_assignment_cases group)
        set(target "umicom-variable-assignment-${group}-test")
        add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/variable_assignment/test_${group}.c")
        target_link_libraries(${target} PRIVATE Umicom::Framework)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
        foreach(case IN LISTS ARGN)
            add_test(NAME framework.variable_assignment.${group}.${case} COMMAND ${target} ${case})
            set_tests_properties(framework.variable_assignment.${group}.${case} PROPERTIES
                TIMEOUT 30 LABELS "framework;debugger;variables;regression")
        endforeach()
    endfunction()
    umicom_assignment_cases(target root scalar empty-name missing-container large-container duplicate-root nested duplicate-child scope invalidate)
    umicom_assignment_cases(reply valid empty unicode evaluate-field wrong-value wrong-type duplicate duplicate-body duplicate-reference
        negative fraction overflow boundary-reference nul surrogate trailing extension boundary-value oversized)
    if(TARGET umicom-dap-protocol-fixture)
        add_executable(umicom-variable-assignment-platform-test "${CMAKE_CURRENT_LIST_DIR}/../tests/variable_assignment/test_platform.c")
        target_link_libraries(umicom-variable-assignment-platform-test PRIVATE Umicom::Framework)
        umicom_apply_warnings(umicom-variable-assignment-platform-test)
        umicom_apply_sanitizers(umicom-variable-assignment-platform-test)
        add_dependencies(umicom-variable-assignment-platform-test umicom-dap-protocol-fixture)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-variable-assignment-platform-test)
        endif()
        foreach(case idle success nested unicode empty boundary-input duplicate prequeued unsupported stale owner zero-timeout long-input invalid-utf8 rejected timeout event command malformed legacy-variable legacy-expression)
            add_test(NAME framework.variable_assignment.platform.${case} COMMAND umicom-variable-assignment-platform-test
                "$<TARGET_FILE:umicom-dap-protocol-fixture>" ${case})
            set_tests_properties(framework.variable_assignment.platform.${case} PROPERTIES
                TIMEOUT 30 LABELS "framework;debugger;variables;protocol-fixture;regression")
        endforeach()
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/assigning-debugger-variables.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
