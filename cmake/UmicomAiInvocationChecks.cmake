# Umicom Framework
# File: cmake/UmicomAiInvocationChecks.cmake
#
# PURPOSE:
# Register complete native AuthorEngine argv planning checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-ai-authorengine-planning-test tests/test_ai_authorengine_planning.c)
    target_link_libraries(umicom-ai-authorengine-planning-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-authorengine-planning-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-authorengine-planning-test)
    umicom_apply_sanitizers(umicom-ai-authorengine-planning-test)
    foreach(_case IN ITEMS publication-commands
            publication-no-provider
            publication-unused-provider
            publication-null-config
            publication-null-output
            publication-empty-executable
            publication-empty-workspace
            publication-executable-bound
            publication-workspace-bound
            publication-long-site
            publication-site-limit
            publication-site-invalid-command
            publication-command-invalid
            publication-alias
            ai-health
            ai-catalogue
            ai-generate
            ai-null-config
            ai-null-output
            ai-command-invalid
            ai-missing-input
            ai-long-session
            ai-long-model
            ai-long-input
            ai-long-output
            ai-long-workspace
            ai-argument-limit
            ai-alias
            ai-alias-refusal
            literal-arguments
            publication-field-limits)
        set(_name "framework.ai.authorengine.planning.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-authorengine-planning-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;authorengine;planning")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-authorengine-planning-test)
    endforeach()
endif()
