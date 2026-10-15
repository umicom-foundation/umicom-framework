# Umicom Framework. Sammy Hegab, Umicom Foundation. MIT licence.
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
add_executable(umicom-sdk-metadata-tests
    "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_installation/test_metadata.c")
add_executable(umicom-sdk-inspection-tests
    "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_installation/test_inspection.c")
foreach(_target umicom-sdk-metadata-tests umicom-sdk-inspection-tests)
    target_link_libraries(${_target} PRIVATE Umicom::sdk)
    umicom_apply_warnings(${_target})
    umicom_apply_sanitizers(${_target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${_target})
    endif()
endforeach()
foreach(_case valid crlf duplicate unknown missing nul bare_cr maximum path_overflow null oversize
        schema version_suffix version_parts version_extra version_empty version_overflow
        abi_zero abi_overflow abi_sign absolute drive parent dot empty_segment trailing_separator
        backslash list expression trailing_dot)
    add_test(NAME "framework.sdk.installation.metadata.${_case}"
        COMMAND umicom-sdk-metadata-tests "${_case}")
    set_tests_properties("framework.sdk.installation.metadata.${_case}" PROPERTIES
        LABELS "framework;sdk;parser;regression" TIMEOUT 15)
endforeach()
foreach(_case complete lib64 multiarch old_version low_abi config targets package_version header
        library library_directory no_metadata legacy malformed)
    add_test(NAME "framework.sdk.installation.observe.${_case}"
        COMMAND "${CMAKE_COMMAND}"
            "-DTEST_PROGRAM=$<TARGET_FILE:umicom-sdk-inspection-tests>"
            "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/sdk-inspection-fixtures"
            "-DCASE=${_case}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_installation/check_installation.cmake")
    set_tests_properties("framework.sdk.installation.observe.${_case}" PROPERTIES
        LABELS "framework;sdk;filesystem;regression" TIMEOUT 30)
    if(TARGET umicom)
        add_test(NAME "framework.sdk.installation.cli.${_case}"
            COMMAND "${CMAKE_COMMAND}" "-DCLI_PROGRAM=$<TARGET_FILE:umicom>"
                "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/sdk-cli-fixtures" "-DCASE=${_case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_installation/check_installation.cmake")
        set_tests_properties("framework.sdk.installation.cli.${_case}" PROPERTIES
            LABELS "framework;sdk;cli;regression" TIMEOUT 30)
    endif()
endforeach()

if(TARGET Umicom::ui_gtk4)
    add_executable(umicom-sdk-settings-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_installation/test_settings_gtk4.c")
    target_link_libraries(umicom-sdk-settings-tests PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-sdk-settings-tests)
    umicom_apply_sanitizers(umicom-sdk-settings-tests)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-sdk-settings-tests)
    endif()
    foreach(_case select merge invalid reentrant edit_back hide incomplete retained)
        set(_fixture complete)
        if(_case STREQUAL "incomplete")
            set(_fixture library)
        endif()
        add_test(NAME "framework.sdk.installation.settings.${_case}"
            COMMAND "${CMAKE_COMMAND}"
                "-DTEST_PROGRAM=$<TARGET_FILE:umicom-sdk-settings-tests>"
                "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/sdk-settings-fixtures"
                "-DCASE=${_fixture}" "-DSDK_GTK_CASE=${_case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_installation/check_installation.cmake")
        set_tests_properties("framework.sdk.installation.settings.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;sdk;gtk4;regression"
            SKIP_REGULAR_EXPRESSION "SDK GTK display unavailable")
    endforeach()
endif()

add_executable(umicom-sdk-path-assembly-tests
    "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_installation/test_path_assembly.c")
target_link_libraries(umicom-sdk-path-assembly-tests PRIVATE Umicom::sdk)
umicom_apply_warnings(umicom-sdk-path-assembly-tests)
umicom_apply_sanitizers(umicom-sdk-path-assembly-tests)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-sdk-path-assembly-tests)
endif()
foreach(_case normal derived_capacity prefix_capacity empty null)
    add_test(NAME "framework.sdk.installation.paths.${_case}" COMMAND umicom-sdk-path-assembly-tests "${_case}")
    set_tests_properties("framework.sdk.installation.paths.${_case}" PROPERTIES
        TIMEOUT 15 LABELS "framework;sdk;regression")
endforeach()
