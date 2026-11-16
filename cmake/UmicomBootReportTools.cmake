# Umicom Framework
# File: cmake/UmicomBootReportTools.cmake
#
# PURPOSE:
#   Export structured boot observations above platform without coupling platform back to protocol.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
set(_umicom_boot_tools_root "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_boot_report_tools STATIC "${_umicom_boot_tools_root}/src/platform/boot_report_json.c")
add_library(Umicom::boot_report_tools ALIAS umicom_boot_report_tools)
set_target_properties(umicom_boot_report_tools PROPERTIES EXPORT_NAME boot_report_tools
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_boot_report_tools PUBLIC
    $<BUILD_INTERFACE:${_umicom_boot_tools_root}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_boot_report_tools PUBLIC Umicom::platform PRIVATE Umicom::protocol)
umicom_apply_warnings(umicom_boot_report_tools)
umicom_apply_sanitizers(umicom_boot_report_tools)
install(TARGETS umicom_boot_report_tools EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_boot_report_tools)
if(BUILD_TESTING)
    add_executable(umicom-boot-report-json-test "${_umicom_boot_tools_root}/tests/os_foundation/test_boot_report_json.c")
    add_executable(umicom-boot-report-json-example-test "${_umicom_boot_tools_root}/examples/boot_report_json/main.c")
    foreach(_target umicom-boot-report-json-test umicom-boot-report-json-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::boot_report_tools)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case ready starting recovery all-reasons maximum equal semantic-padding progress source all-fields same-object capacity comparison-capacity invalid-mode invalid-state invalid-counts invalid-reason invalid-source unterminated invalid-before invalid-after nulls)
        add_test(NAME framework.os_foundation.boot_report_json.${_case} COMMAND umicom-boot-report-json-test ${_case})
        set_tests_properties(framework.os_foundation.boot_report_json.${_case} PROPERTIES
            TIMEOUT 20 LABELS "framework;os-foundation;json;sdk")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.os_foundation.boot_report_json.${_case} TARGETS umicom-boot-report-json-test)
        endif()
    endforeach()
    add_test(NAME framework.os_foundation.boot_report_json.sdk-example COMMAND umicom-boot-report-json-example-test)
    set_tests_properties(framework.os_foundation.boot_report_json.sdk-example PROPERTIES
        TIMEOUT 20 LABELS "framework;os-foundation;json;sdk")
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    umicom_apply_application_branding(TARGET umicom-boot-report-json-example-test
        PRODUCT_NAME "Umicom Captured Boot JSON Example" INTERNAL_NAME "umicom-boot-report-json-example-test"
        APPLICATION_ID "org.umicom.boot-report-json-example"
        RESOURCE_ROOT "${_umicom_boot_tools_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_boot_tools_root}/examples/boot_report_json/main.c"
        "${_umicom_boot_tools_root}/examples/boot_report_json/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/boot_report_json"
        COMPONENT "${_component}" ${_selection})
endforeach()
