# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Owned child captures are shared by the runtime and product workbench.
include_guard(GLOBAL)
target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/variable_inspection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/variable_reply.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json_document.c")

if(BUILD_TESTING)
    function(umicom_variable_cases group)
        set(target "umicom-variable-inspection-${group}-test")
        add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/variable_inspection/test_${group}.c")
        target_link_libraries(${target} PRIVATE Umicom::Framework)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
        foreach(case IN LISTS ARGN)
            add_test(NAME framework.variable_inspection.${group}.${case} COMMAND ${target} ${case})
            set_tests_properties(framework.variable_inspection.${group}.${case} PROPERTIES
                TIMEOUT 30 LABELS "framework;debugger;variables;regression")
        endforeach()
    endfunction()
    umicom_variable_cases(document grammar extension-invalid extension-unicode whitespace depth depth-limit byte-limit token-limit atomic)
    umicom_variable_cases(capture owned bounds owner console scalar large-reference variable reuse running frame scope session)
    umicom_variable_cases(page invalid depth empty cycle names capacity owned bounds stale-child)
    umicom_variable_cases(reply valid unicode empty empty-value missing array-type row-type missing-value missing-reference
        duplicate duplicate-array duplicate-body nul surrogate optional-type negative-count negative-reference fraction overflow
        boundary-reference long-value boundary-value capacity oversized names)
    if(TARGET umicom-dap-protocol-fixture)
        add_executable(umicom-variable-inspection-platform-test "${CMAKE_CURRENT_LIST_DIR}/../tests/variable_inspection/test_platform.c")
        target_link_libraries(umicom-variable-inspection-platform-test PRIVATE Umicom::Framework)
        umicom_apply_warnings(umicom-variable-inspection-platform-test)
        umicom_apply_sanitizers(umicom-variable-inspection-platform-test)
        add_dependencies(umicom-variable-inspection-platform-test umicom-dap-protocol-fixture)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-variable-inspection-platform-test)
        endif()
        foreach(case idle wrong-owner stale scalar success small-stack nested repeat empty unicode cycle rejected malformed command event timeout)
            add_test(NAME framework.variable_inspection.platform.${case} COMMAND umicom-variable-inspection-platform-test
                "$<TARGET_FILE:umicom-dap-protocol-fixture>" ${case})
            set_tests_properties(framework.variable_inspection.platform.${case} PROPERTIES
                TIMEOUT 30 LABELS "framework;debugger;variables;protocol-fixture;regression")
        endforeach()
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/variable-inspection.md"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
