# Umicom Framework
# File: cmake/UmicomTreasuryForwardUi.cmake
# PURPOSE: Attach the shared treasury practice surface after native dependencies exist.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
include_guard(GLOBAL)
function(umicom_attach_treasury_forward_ui)
    # Native persistence uses the separately exported file adapter.
    if(NOT TARGET umicom_ui_gtk4 OR NOT TARGET Umicom::treasury_files)
        return()
    endif()
    get_target_property(attached umicom_ui_gtk4 UMICOM_FORWARD_PRACTICE_ATTACHED)
    if(attached)
        return()
    endif()
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/forward_practice_gtk4.c")
    # The earlier finance-only link is preserved; treasury_files carries finance
    # and platform transitively without changing the portable finance library.
    if(FALSE)
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::finance)
    endif()
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::treasury_files)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom_ui_gtk4 FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/forward_practice_files.inc")
    endif()
    target_compile_definitions(umicom_ui_gtk4 PUBLIC UMICOM_HAS_FORWARD_PRACTICE=1)
    set_property(TARGET umicom_ui_gtk4 PROPERTY UMICOM_FORWARD_PRACTICE_ATTACHED TRUE)
    if(BUILD_TESTING)
        add_executable(umi-forward-practice-native-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/open_tms/test_forward_practice_native.c")
        target_link_libraries(umi-forward-practice-native-test PRIVATE Umicom::ui_gtk4)
        set_target_properties(umi-forward-practice-native-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umi-forward-practice-native-test)
        umicom_apply_sanitizers(umi-forward-practice-native-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umi-forward-practice-native-test)
        endif()
        foreach(case capture duplicate invalid-nan invalid-hex invalid-space invalid-comma
            invalid-currency invalid-same-currency invalid-notional zero-market skip unacknowledged
            apply lifecycle cancel changed-next changed-form changed-selected edit-preview close-preview
            ack-edit ack-click capture-nested parent invalid-api)
            set(test framework.open_tms.forward.native.${case})
            add_test(NAME ${test} COMMAND umi-forward-practice-native-test ${case})
            set_tests_properties(${test} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
                ENVIRONMENT "G_DEBUG=fatal-warnings"
                LABELS "framework;finance;treasury;review;gtk4;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${test} TARGETS umi-forward-practice-native-test)
            endif()
        endforeach()
    endif()

    # Worker ownership and explicit replacement are native workflow contracts.
    if(BUILD_TESTING)
        add_executable(umi-forward-files-native-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/open_tms/test_forward_files_native.c")
        target_link_libraries(umi-forward-files-native-test PRIVATE Umicom::ui_gtk4)
        set_target_properties(umi-forward-files-native-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umi-forward-files-native-test)
        umicom_apply_sanitizers(umi-forward-files-native-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umi-forward-files-native-test)
        endif()
        foreach(case save existing relative missing malformed invalid-utf8 review unacknowledged
            apply replay dirty-load edited-review revokes-transition edit-preview close-preview
            ack-edit ack-click start-close start-edit close-pending save-snapshot save-close
            invalid-api maturity)
            set(test framework.open_tms.forward.files-native.${case})
            add_test(NAME ${test} COMMAND umi-forward-files-native-test ${case})
            set_tests_properties(${test} PROPERTIES TIMEOUT 40 SKIP_RETURN_CODE 77
                ENVIRONMENT "G_DEBUG=fatal-warnings"
                LABELS "framework;finance;treasury;files;gtk4;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${test} TARGETS umi-forward-files-native-test)
            endif()
        endforeach()
    endif()
endfunction()
umicom_attach_treasury_forward_ui()
# Retry composition if either native or optional file dependencies appear later.
if(NOT TARGET umicom_ui_gtk4 OR NOT TARGET umicom_treasury_files)
    cmake_language(DEFER CALL umicom_attach_treasury_forward_ui)
endif()
