# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Add a native UTF-8 replacement contract without changing legacy save policy.
include_guard(GLOBAL)
target_sources(umicom_platform PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/platform/local_replace.c")
if(BUILD_TESTING)
    add_executable(umicom-local-file-replace-test "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_local_replace.c")
    target_link_libraries(umicom-local-file-replace-test PRIVATE Umicom::platform)
    umicom_apply_warnings(umicom-local-file-replace-test)
    umicom_apply_sanitizers(umicom-local-file-replace-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-local-file-replace-test)
    endif()
    foreach(case replace unicode repeat empty invalid directory symlink missing-parent)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/local-replace/${case}")
        add_test(NAME framework.local_file_replace.${case} COMMAND umicom-local-file-replace-test ${case})
        set_tests_properties(framework.local_file_replace.${case} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/local-replace/${case}"
            LABELS "framework;platform;filesystem;regression")
    endforeach()
endif()
