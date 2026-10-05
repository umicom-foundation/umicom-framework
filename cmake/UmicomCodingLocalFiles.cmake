# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Exercise the shared file adapter with actual isolated native files.
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-coding-local-files-test "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_local_files.c")
    target_link_libraries(umicom-coding-local-files-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-coding-local-files-test)
    umicom_apply_sanitizers(umicom-coding-local-files-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-coding-local-files-test)
    endif()
    foreach(case roundtrip unicode capture binary capacity empty missing remove invalid resolve)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/coding-local-files/${case}")
        add_test(NAME framework.ai_coding_runtime.local_files.${case} COMMAND umicom-coding-local-files-test ${case})
        set_tests_properties(framework.ai_coding_runtime.local_files.${case} PROPERTIES TIMEOUT 60
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/coding-local-files/${case}"
            LABELS "framework;developer;coding;filesystem;regression")
    endforeach()
endif()
