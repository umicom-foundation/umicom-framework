#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWebPreviewChecks.cmake
#
# PURPOSE:
#   Register composed preview, command admission and Unicode native-entry regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING AND (WIN32 OR UNIX) AND NOT EMSCRIPTEN)
    foreach(suite preview preview_cli)
        add_executable(umicom-web-${suite}-test "${CMAKE_CURRENT_LIST_DIR}/../tests/web/test_${suite}.c")
        target_link_libraries(umicom-web-${suite}-test PRIVATE Umicom::web)
        set_target_properties(umicom-web-${suite}-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-web-${suite}-test)
        umicom_apply_sanitizers(umicom-web-${suite}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-web-${suite}-test)
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-web-preview_cli-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_preview.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/cli.h")
    endif()
    set(preview_cases null relative-root low-port high-port long-root port-min port-max port-leading-zero port-empty port-sign port-space port-overflow port-text port-null create-invalid create-null-output index missing traversal post head foreign-host foreign-origin same-origin duplicate-host copied-root unicode-root binary stopped-handle serve-before-start)
    set(preview_cli_cases help missing-root missing-port root-value port-value duplicate-root duplicate-port duplicate-dry-run null-vector null-item empty-item negative-argc unknown late-unknown relative-root overflow-port low-port high-port zero-port empty-argv dry-run order create-failure start-failure signal-int-failure signal-term-failure normal-stop client-timeout client-close bad-request listener-failure restore-failure)

    foreach(suite preview preview_cli)
        foreach(case IN LISTS ${suite}_cases)
            add_test(NAME framework.web.${suite}.${case} COMMAND umicom-web-${suite}-test "${case}")
            set_tests_properties(framework.web.${suite}.${case} PROPERTIES TIMEOUT 20 LABELS "framework;web;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(framework.web.${suite}.${case} TARGETS umicom-web-${suite}-test)
            endif()
        endforeach()
    endforeach()
    if(TARGET umicom)
        foreach(case unicode relative invalid-port)
            add_test(NAME framework.web.preview_entry.${case} COMMAND "${CMAKE_COMMAND}"
                "-DTOOL=$<TARGET_FILE:umicom>" "-DCASE=${case}"
                "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/preview-entry-${case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/web/check_preview_command.cmake")
            set_tests_properties(framework.web.preview_entry.${case} PROPERTIES TIMEOUT 20 LABELS "framework;web;cli;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(framework.web.preview_entry.${case} TARGETS umicom)
            endif()
        endforeach()
    endif()

    add_executable(umicom-web-local-service-entry-test "${CMAKE_CURRENT_LIST_DIR}/../tests/web/test_local_service_entry.c")
    target_link_libraries(umicom-web-local-service-entry-test PRIVATE Umicom::web)
    set_target_properties(umicom-web-local-service-entry-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-web-local-service-entry-test)
    umicom_apply_sanitizers(umicom-web-local-service-entry-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-web-local-service-entry-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-web-local-service-entry-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../examples/web/local_service.c")
    endif()
    foreach(case unicode ascii relative invalid-port)
        add_test(NAME framework.web.local_service_entry.${case} COMMAND "${CMAKE_COMMAND}"
            "-DTOOL=$<TARGET_FILE:umicom-web-local-service-entry-test>" "-DCASE=${case}"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/local-service-entry-${case}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/web/check_local_service_entry.cmake")
        set_tests_properties(framework.web.local_service_entry.${case} PROPERTIES TIMEOUT 20 LABELS "framework;web;native;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.web.local_service_entry.${case} TARGETS umicom-web-local-service-entry-test)
        endif()
    endforeach()
endif()
