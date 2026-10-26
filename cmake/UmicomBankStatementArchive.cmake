
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Scope persistence belongs to bank_feed; applications reuse the existing host.
include_guard(GLOBAL)
target_sources(umicom_bank_feed PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/bank_feed/statement_archive.c")
if(BUILD_TESTING)
    add_executable(umi-bank-statement-archive-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_statement_archive.c")
    set_target_properties(umi-bank-statement-archive-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umi-bank-statement-archive-test PRIVATE Umicom::bank_feed)
    umicom_apply_warnings(umi-bank-statement-archive-test)
    umicom_apply_sanitizers(umi-bank-statement-archive-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-bank-statement-archive-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umi-bank-statement-archive-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/csv_fixture.h")
    endif()
    foreach(case roundtrip mismatch mixed-scale negative empty-period integer-minimum
        unknown-source create-refusals recalculate root-unknown root-duplicate root-missing
        version version-kind format invalid-date reversed-period unknown-account currency-kind
        noncanonical decimal-exponent decimal-precision overflow nested-unknown nested-duplicate
        nested-amount nested-source pagination truncated embedded-nul bounds invalid-api)
        set(test framework.bank_statement.archive.${case})
        add_test(NAME ${test} COMMAND umi-bank-statement-archive-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30
            LABELS "framework;banking;reconciliation;persistence;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-bank-statement-archive-test)
        endif()
    endforeach()
    unset(test)
endif()

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    foreach(kind native workflow)
        add_executable(umi-statement-archive-${kind}-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/test_statement_archive_${kind}.c")
        set_target_properties(umi-statement-archive-${kind}-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        target_link_libraries(umi-statement-archive-${kind}-test PRIVATE Umicom::ui_gtk4)
        umicom_apply_warnings(umi-statement-archive-${kind}-test)
        umicom_apply_sanitizers(umi-statement-archive-${kind}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umi-statement-archive-${kind}-test)
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(umi-statement-archive-${kind}-test FILES
                "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_feed/fixture.h"
                "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
        endif()
        if(kind STREQUAL "native")
            set(cases roundtrip mismatch legacy invalid-source invalid edit retained parent
                edit-notify close-notify nested-compare exclusive)
        else()
            set(cases roundtrip independent parent invalid format-change path-change close-loading edit-notify close-notify)
        endif()
        foreach(case IN LISTS cases)
            set(test framework.bank_statement.archive.${kind}.${case})
            add_test(NAME ${test} COMMAND umi-statement-archive-${kind}-test ${case})
            set_tests_properties(${test} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
                ENVIRONMENT "G_DEBUG=fatal-warnings"
                LABELS "framework;banking;reconciliation;persistence;gtk4;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${test} TARGETS umi-statement-archive-${kind}-test)
            endif()
        endforeach()
    endforeach()
    unset(cases)
    unset(test)
endif()

if(BUILD_TESTING)
    add_executable(umi-statement-review-consumer
        "${CMAKE_CURRENT_LIST_DIR}/../examples/statement_review/main.c")
    target_link_libraries(umi-statement-review-consumer PRIVATE Umicom::bank_feed)
    set_target_properties(umi-statement-review-consumer PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-statement-review-consumer)
    umicom_apply_sanitizers(umi-statement-review-consumer)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-statement-review-consumer)
    endif()
    foreach(scenario roundtrip mismatch)
        set(test framework.bank_statement.archive.consumer.${scenario})
        add_test(NAME ${test} COMMAND umi-statement-review-consumer ${scenario})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30
            LABELS "framework;banking;reconciliation;sdk;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-statement-review-consumer)
        endif()
    endforeach()
    unset(test)
endif()
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/statement_review/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/statement_review/CMakeLists.txt"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/statement_review COMPONENT Learning)
