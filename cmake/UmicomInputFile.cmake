# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# The regular-file reader belongs to Platform, so its native-file coverage
# remains available in Framework and OS builds without any AI applications.
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-input-file-test "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_input_file.c")
    target_link_libraries(umicom-input-file-test PRIVATE Umicom::platform)
    umicom_apply_warnings(umicom-input-file-test)
    umicom_apply_sanitizers(umicom-input-file-test)
    foreach(case bytes empty unicode limit large missing directory arguments fifo symlink)
        add_test(NAME framework.platform.input_file.${case} COMMAND umicom-input-file-test ${case})
        set_tests_properties(framework.platform.input_file.${case} PROPERTIES TIMEOUT 15 SKIP_RETURN_CODE 77 LABELS "framework;platform;file;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-input-file-test)
    endif()
endif()
