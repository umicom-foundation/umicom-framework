# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Register shared report regressions against their existing library owners.
include_guard(GLOBAL)
if(BUILD_TESTING)
    function(umicom_report_export_test group source library)
        set(target "umicom-report-export-${group}-test")
        add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/report_export/${source}")
        target_link_libraries(${target} PRIVATE ${library})
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        foreach(case IN LISTS ARGN)
            add_test(NAME "framework.report_export.${group}.${case}" COMMAND ${target} ${case})
            set_tests_properties("framework.report_export.${group}.${case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;report;csv;regression")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endfunction()
    umicom_report_export_test(csv test_csv_document.c Umicom::base
        escaping formula numbers capacity invalid-text copy-alias columns locale invalid)
    umicom_report_export_test(orders test_order_csv.c Umicom::trading_ui
        filter empty formula ownership invalid)
    umicom_report_export_test(evidence test_evidence_csv.c Umicom::test_platform
        selection empty retention invalid-text ownership)
endif()

if(BUILD_TESTING AND TARGET Umicom::ui_gtk4)
    add_executable(umicom-report-export-orders-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/report_export/test_order_csv_native.c")
    target_link_libraries(umicom-report-export-orders-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-report-export-orders-native-test)
    umicom_apply_sanitizers(umicom-report-export-orders-native-test)
    add_test(NAME framework.report_export.orders.native COMMAND umicom-report-export-orders-native-test)
    set_tests_properties(framework.report_export.orders.native PROPERTIES
        TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;trading;csv;gtk4;regression")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-report-export-orders-native-test)
    endif()
endif()
