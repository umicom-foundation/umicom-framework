# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Shared immutable comparison belongs to the existing bank-feed SDK owner.
include_guard(GLOBAL)
target_sources(umicom_bank_feed PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/comparison.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/comparison_report.c")
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_bank_feed FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/comparison_private.h")
endif()
if(BUILD_TESTING)
    foreach(kind comparison comparison_report)
        string(REPLACE "_" "-" target_kind "${kind}")
        set(target "umi-bank-${target_kind}-test")
        add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_${kind}.c")
        set_target_properties(${target} PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        target_link_libraries(${target} PRIVATE Umicom::bank_feed)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${target} FILES
                "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/fixture.h"
                "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/csv_fixture.h")
        endif()
        if(kind STREQUAL "comparison")
            set(cases same copied-row amount amount-spelling currency direction status
                mutability booking-time reference information reordered one-sided before-empty
                after-empty missing-id account case-sensitive same-input empty pagination capacity invalid-api)
        else()
            set(cases normal empty formula unidentified one-sided capacity invalid-api)
        endif()
        foreach(case IN LISTS cases)
            set(test "framework.bank_feed.${kind}.${case}")
            add_test(NAME ${test} COMMAND ${target} ${case})
            set_tests_properties(${test} PROPERTIES TIMEOUT 30
                LABELS "framework;banking;comparison;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${test} TARGETS ${target})
            endif()
        endforeach()
    endforeach()
    unset(test)
    unset(target)
    unset(target_kind)
    unset(cases)
endif()

if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/bank_comparison_gtk4.c")
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/bank_comparison.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4 COMPONENT Framework)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom_ui_gtk4 FILES
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/bank_feed_comparison.inc")
    endif()
endif()

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umi-bank-comparison-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_comparison_native.c")
    set_target_properties(umi-bank-comparison-native-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umi-bank-comparison-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umi-bank-comparison-native-test)
    umicom_apply_sanitizers(umi-bank-comparison-native-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-bank-comparison-native-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umi-bank-comparison-native-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/csv_fixture.h")
    endif()
    foreach(case normal filter empty invalid-selection filter-close filter-reentry
        detail-close detail-reentry parent retained invalid-input removed-parent)
        set(test framework.bank_feed.comparison.native.${case})
        add_test(NAME ${test} COMMAND umi-bank-comparison-native-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
            ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "framework;banking;comparison;gtk4;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-bank-comparison-native-test)
        endif()
    endforeach()
    unset(test)
endif()

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umi-bank-comparison-workflow-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_comparison_workflow.c")
    set_target_properties(umi-bank-comparison-workflow-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umi-bank-comparison-workflow-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umi-bank-comparison-workflow-test)
    umicom_apply_sanitizers(umi-bank-comparison-workflow-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-bank-comparison-workflow-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umi-bank-comparison-workflow-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/csv_fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
    endif()
    foreach(case normal empty clear-current invalid-after loading replace-before
        forget retained-close retain-close retain-reentry)
        set(test framework.bank_feed.comparison.workflow.${case})
        add_test(NAME ${test} COMMAND umi-bank-comparison-workflow-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
            ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "framework;banking;comparison;gtk4;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-bank-comparison-workflow-test)
        endif()
    endforeach()
    unset(test)
endif()

install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/bank_feed/comparison-before.csv"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/bank_feed/comparison-after.csv"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/bank_feed COMPONENT Learning)

if(BUILD_TESTING)
    add_executable(umi-bank-comparison-consumer
        "${CMAKE_CURRENT_LIST_DIR}/../examples/bank_comparison/main.c")
    target_link_libraries(umi-bank-comparison-consumer PRIVATE Umicom::bank_feed)
    set_target_properties(umi-bank-comparison-consumer PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-bank-comparison-consumer)
    umicom_apply_sanitizers(umi-bank-comparison-consumer)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-bank-comparison-consumer)
    endif()
    foreach(scenario compare missing-id)
        add_test(NAME framework.bank_feed.comparison.consumer.${scenario}
            COMMAND umi-bank-comparison-consumer ${scenario})
        set_tests_properties(framework.bank_feed.comparison.consumer.${scenario} PROPERTIES
            TIMEOUT 30 LABELS "framework;banking;comparison;sdk;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.bank_feed.comparison.consumer.${scenario}
                TARGETS umi-bank-comparison-consumer)
        endif()
    endforeach()
endif()
