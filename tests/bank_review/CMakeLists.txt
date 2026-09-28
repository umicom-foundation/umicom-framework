# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
add_executable(umicom-bank-review-test "${CMAKE_CURRENT_LIST_DIR}/test_review.c")
target_link_libraries(umicom-bank-review-test PRIVATE Umicom::bank_operations)
umicom_bank_review_target(umicom-bank-review-test)
foreach(_case IN ITEMS reference_cycles maximum_accounts event_limit arguments preview apply copied_command snapshot_copy source_lifetime
    revocation identity_change capability_change stale_local same_revision_other_history equivalent_history
    repeat_apply field_binding text text_capacity draft_discard review_not_reservation
    insufficient invalid_currency self_approval unapproved_execution blocked_account conflicting_request idempotent
    approval reject cancel execution reversal hold release capture refund eligibility new_account customer_eligibility
    reconcile_match reconcile_break
    sqlite_restart sqlite_stale_writer sqlite_write_failure sqlite_review_read_only
    sqlite_history_changed sqlite_history_missing sqlite_orphan sqlite_corrupt)
    add_test(NAME framework.bank_review.${_case} COMMAND umicom-bank-review-test ${_case}
        "${CMAKE_CURRENT_BINARY_DIR}/bank-review-${_case}.sqlite")
    set_tests_properties(framework.bank_review.${_case} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77 LABELS "bank-review;native")
endforeach()
add_test(NAME framework.bank_review.lesson COMMAND umicom-bank-review --self-test)
add_test(NAME framework.bank_review.cli_demo COMMAND umicom-bank-review demo)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(umicom-bank-review-allocation-test "${CMAKE_CURRENT_LIST_DIR}/test_allocation.c")
    target_link_libraries(umicom-bank-review-allocation-test PRIVATE Umicom::bank_operations)
    target_link_options(umicom-bank-review-allocation-test PRIVATE -Wl,--wrap=malloc -Wl,--wrap=calloc)
    umicom_bank_review_target(umicom-bank-review-allocation-test)
    add_test(NAME framework.bank_review.allocation_failures COMMAND umicom-bank-review-allocation-test)
endif()

if(TARGET Umicom::bank_operations_gtk4)
    add_executable(umicom-bank-review-gtk-test "${CMAKE_CURRENT_LIST_DIR}/test_gtk_review.c")
    # The adapter source is compiled in this unit host, not linked a second time.
    target_link_libraries(umicom-bank-review-gtk-test PRIVATE Umicom::bank_operations Umicom::ui_gtk4)
    umicom_bank_review_target(umicom-bank-review-gtk-test)
    foreach(_case IN ITEMS invalidate edit identity no_review close independent)
        add_test(NAME framework.bank_review.gtk_${_case} COMMAND umicom-bank-review-gtk-test ${_case})
        set_tests_properties(framework.bank_review.gtk_${_case} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "bank-review;gtk4")
    endforeach()
endif()
