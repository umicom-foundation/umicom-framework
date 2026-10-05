# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Shared preferences reuse the developer JSON and platform file owners.
include_guard(GLOBAL)
target_sources(umicom_developer PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/adapter_preferences.c")
if(BUILD_TESTING)
    add_executable(umicom-debug-adapter-preferences-test "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_preferences/test_preferences.c")
    target_link_libraries(umicom-debug-adapter-preferences-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-debug-adapter-preferences-test)
    umicom_apply_sanitizers(umicom-debug-adapter-preferences-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-debug-adapter-preferences-test)
    endif()
    foreach(case round-trip path-default atomic-output invalid-fields documents save-load invalid-save user-path)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/debug-preferences/${case}")
        add_test(NAME framework.debug_adapter_preferences.${case} COMMAND umicom-debug-adapter-preferences-test ${case})
        set_tests_properties(framework.debug_adapter_preferences.${case} PROPERTIES TIMEOUT 30
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/debug-preferences/${case}"
            LABELS "framework;debugger;preferences;regression")
    endforeach()
endif()
