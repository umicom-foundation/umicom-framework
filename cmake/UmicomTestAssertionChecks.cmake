# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# The fixture must retain setup calls even when Release flags define NDEBUG.
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-test-assertions-policy
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/test_assertions.c")
    set_target_properties(umicom-test-assertions-policy PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_compile_definitions(umicom-test-assertions-policy PRIVATE NDEBUG)
    umicom_apply_warnings(umicom-test-assertions-policy)
    umicom_apply_sanitizers(umicom-test-assertions-policy)
    add_test(NAME framework.testing.release_assertions
        COMMAND umicom-test-assertions-policy)
    set_tests_properties(framework.testing.release_assertions PROPERTIES
        TIMEOUT 15 LABELS "framework;testing;release;regression")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-test-assertions-policy)
    endif()
endif()
