# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
add_executable(umicom-finance-close-review-test "${CMAKE_CURRENT_LIST_DIR}/test_close.c")
target_link_libraries(umicom-finance-close-review-test PRIVATE Umicom::finance_operations)
umicom_finance_operations_configure_target(umicom-finance-close-review-test)
foreach(_case IN ITEMS arguments empty_period initial_reconciliation draft approved open_order cancel_order
    matched cleared settled mismatch stale latest_evidence prior_period future_order end_date_inclusive
    two_currencies reversed_history immutable outlives_service no_mutation format format_capacity
    query_bounds prepare_finalise same_actor closed recheck_after_review blocked_reload poisoned
    corrupt_count corrupt_lines policy_agreement stored_prefix_changed stored_prefix_missing stored_prefix_extra)
    add_test(NAME framework.finance_close_review.${_case} COMMAND umicom-finance-close-review-test ${_case})
    set_tests_properties(framework.finance_close_review.${_case} PROPERTIES TIMEOUT 30)
endforeach()
add_test(NAME framework.finance_close_review.lesson COMMAND umicom-finance-close-review --self-test)
add_test(NAME framework.finance_close_review.demo COMMAND umicom-finance-close-review demo)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(umicom-finance-close-alloc-test "${CMAKE_CURRENT_LIST_DIR}/test_alloc.c")
    target_link_libraries(umicom-finance-close-alloc-test PRIVATE Umicom::finance_operations)
    target_link_options(umicom-finance-close-alloc-test PRIVATE -Wl,--wrap=calloc)
    umicom_finance_operations_configure_target(umicom-finance-close-alloc-test)
    add_test(NAME framework.finance_close_review.allocation_failure COMMAND umicom-finance-close-alloc-test)
endif()

if(TARGET Umicom::finance_operations_gtk4)
    add_executable(umicom-finance-close-gtk-test "${CMAKE_CURRENT_LIST_DIR}/test_gtk.c")
    target_link_libraries(umicom-finance-close-gtk-test PRIVATE Umicom::finance_operations_gtk4)
    umicom_finance_operations_configure_target(umicom-finance-close-gtk-test)
    add_test(NAME framework.finance_close_review.gtk_report COMMAND umicom-finance-close-gtk-test)
    set_tests_properties(framework.finance_close_review.gtk_report PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77)
endif()
