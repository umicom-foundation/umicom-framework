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

# Diagnostic cases use the same native writer and isolated working directories.
# Windows cases redirect only publication and waiting to avoid timing races.
if(BUILD_TESTING)
    add_executable(umicom-local-file-replace-report-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_local_replace_report.c")
    target_link_libraries(umicom-local-file-replace-report-test PRIVATE Umicom::platform)
    umicom_apply_warnings(umicom-local-file-replace-report-test)
    umicom_apply_sanitizers(umicom-local-file-replace-report-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-local-file-replace-report-test)
    endif()
    foreach(case invalid-report invalid-bytes-report directory-report empty-report)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/local-replace/${case}")
        add_test(NAME framework.local_file_replace.${case}
            COMMAND umicom-local-file-replace-report-test ${case})
        set_tests_properties(framework.local_file_replace.${case} PROPERTIES TIMEOUT 30
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/local-replace/${case}"
            LABELS "framework;platform;filesystem;regression")
    endforeach()
    if(WIN32)
        add_executable(umicom-local-file-replace-retry-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_local_replace_retry.c")
        target_link_libraries(umicom-local-file-replace-retry-test PRIVATE Umicom::platform)
        umicom_apply_warnings(umicom-local-file-replace-retry-test)
        umicom_apply_sanitizers(umicom-local-file-replace-retry-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-local-file-replace-retry-test)
        endif()
        foreach(case sharing-recovered lock-recovered sharing-exhausted access-denied
                     missing-source destination-changed cleanup-refused native-reader)
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/local-replace/${case}")
            add_test(NAME framework.local_file_replace.${case}
                COMMAND umicom-local-file-replace-retry-test ${case})
            set_tests_properties(framework.local_file_replace.${case} PROPERTIES TIMEOUT 30
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/local-replace/${case}"
                LABELS "framework;platform;filesystem;regression;windows")
        endforeach()
    endif()
endif()
