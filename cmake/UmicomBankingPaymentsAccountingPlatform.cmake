#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBankingPaymentsAccountingPlatform.cmake
#
# PURPOSE:
#   Extend canonical Umicom::finance with banking, payments, general-ledger
#   and financial-control foundations used by thin financial applications.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
set(UMICOM_BANKING_PAYMENTS_ACCOUNTING_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_finance)
    message(FATAL_ERROR "UmicomBankingPaymentsAccountingPlatform.cmake requires canonical umicom_finance")
endif()
# Banking, payments and accounting remain reusable Framework finance capabilities.
target_sources(umicom_finance PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/money_rate.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_quote.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_quote_report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/customer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/customer_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/kyc_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/customer_segment.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/bank_product.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/bank_product_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/deposit_account.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/deposit_account_book.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/deposit_transaction.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/interest_rate.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/interest_accrual.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/term_deposit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/loan_account.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/loan_schedule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/loan_installment.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/credit_facility.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/overdraft_facility.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/account_hold.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/bank_relationship.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/banking_snapshot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/banking/banking_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_party.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_instruction.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_batch.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_validation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/approval_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/idempotency_record.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_fee_rule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_charge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_route.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_rail.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_rail_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/cut_off_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_message.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/iso_message.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_status_history.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_clearing.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_settlement.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_return.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_repair.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payment_reconciliation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/payments/payments_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/ledger_account.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/chart_of_accounts.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/account_hierarchy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/accounting_period.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/accounting_calendar.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/accounting_event.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/posting_rule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/journal_line.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/journal_entry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/journal_book.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/double_entry_validation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/posting_engine.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/ledger_balance.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/trial_balance.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/subledger.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/reconciliation_rule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/reconciliation_item.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/reconciliation_run.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/suspense_account.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/financial_control.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/accounting_service.c"
)
# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add banking payments accounting test build helper so parent and application
    # projects apply one consistent rule.
    function(umicom_add_banking_payments_accounting_test target test_name source domain_label)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_BANKING_PAYMENTS_ACCOUNTING_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::finance)
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES LABELS "framework;${domain_label}")
    endfunction()
umicom_add_banking_payments_accounting_test(
    umicom-banking-types-test
    framework.banking.types
    tests/finance_banking/test_types.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-customer-test
    framework.banking.customer
    tests/finance_banking/test_customer.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-customer-registry-test
    framework.banking.customer.registry
    tests/finance_banking/test_customer_registry.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-kyc-profile-test
    framework.banking.kyc.profile
    tests/finance_banking/test_kyc_profile.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-customer-segment-test
    framework.banking.customer.segment
    tests/finance_banking/test_customer_segment.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-bank-product-test
    framework.banking.bank.product
    tests/finance_banking/test_bank_product.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-bank-product-catalogue-test
    framework.banking.bank.product.catalogue
    tests/finance_banking/test_bank_product_catalogue.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-deposit-account-test
    framework.banking.deposit.account
    tests/finance_banking/test_deposit_account.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-deposit-account-book-test
    framework.banking.deposit.account.book
    tests/finance_banking/test_deposit_account_book.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-deposit-transaction-test
    framework.banking.deposit.transaction
    tests/finance_banking/test_deposit_transaction.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-interest-rate-test
    framework.banking.interest.rate
    tests/finance_banking/test_interest_rate.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-interest-accrual-test
    framework.banking.interest.accrual
    tests/finance_banking/test_interest_accrual.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-term-deposit-test
    framework.banking.term.deposit
    tests/finance_banking/test_term_deposit.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-loan-account-test
    framework.banking.loan.account
    tests/finance_banking/test_loan_account.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-loan-schedule-test
    framework.banking.loan.schedule
    tests/finance_banking/test_loan_schedule.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-loan-installment-test
    framework.banking.loan.installment
    tests/finance_banking/test_loan_installment.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-credit-facility-test
    framework.banking.credit.facility
    tests/finance_banking/test_credit_facility.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-overdraft-facility-test
    framework.banking.overdraft.facility
    tests/finance_banking/test_overdraft_facility.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-account-hold-test
    framework.banking.account.hold
    tests/finance_banking/test_account_hold.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-bank-relationship-test
    framework.banking.bank.relationship
    tests/finance_banking/test_bank_relationship.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-banking-snapshot-test
    framework.banking.banking.snapshot
    tests/finance_banking/test_banking_snapshot.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-banking-banking-service-test
    framework.banking.banking.service
    tests/finance_banking/test_banking_service.c
    banking-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-types-test
    framework.payments.types
    tests/finance_payments/test_types.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-party-test
    framework.payments.payment.party
    tests/finance_payments/test_payment_party.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-instruction-test
    framework.payments.payment.instruction
    tests/finance_payments/test_payment_instruction.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-batch-test
    framework.payments.payment.batch
    tests/finance_payments/test_payment_batch.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-validation-test
    framework.payments.payment.validation
    tests/finance_payments/test_payment_validation.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-approval-policy-test
    framework.payments.approval.policy
    tests/finance_payments/test_approval_policy.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-idempotency-record-test
    framework.payments.idempotency.record
    tests/finance_payments/test_idempotency_record.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-fee-rule-test
    framework.payments.payment.fee.rule
    tests/finance_payments/test_payment_fee_rule.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-charge-test
    framework.payments.payment.charge
    tests/finance_payments/test_payment_charge.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-route-test
    framework.payments.payment.route
    tests/finance_payments/test_payment_route.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-rail-test
    framework.payments.payment.rail
    tests/finance_payments/test_payment_rail.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-rail-catalogue-test
    framework.payments.payment.rail.catalogue
    tests/finance_payments/test_payment_rail_catalogue.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-cut-off-policy-test
    framework.payments.cut.off.policy
    tests/finance_payments/test_cut_off_policy.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-message-test
    framework.payments.payment.message
    tests/finance_payments/test_payment_message.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-iso-message-test
    framework.payments.iso.message
    tests/finance_payments/test_iso_message.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-status-history-test
    framework.payments.payment.status.history
    tests/finance_payments/test_payment_status_history.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-clearing-test
    framework.payments.payment.clearing
    tests/finance_payments/test_payment_clearing.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-settlement-test
    framework.payments.payment.settlement
    tests/finance_payments/test_payment_settlement.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-return-test
    framework.payments.payment.return
    tests/finance_payments/test_payment_return.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-repair-test
    framework.payments.payment.repair
    tests/finance_payments/test_payment_repair.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payment-reconciliation-test
    framework.payments.payment.reconciliation
    tests/finance_payments/test_payment_reconciliation.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-payments-payments-service-test
    framework.payments.payments.service
    tests/finance_payments/test_payments_service.c
    payments-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-types-test
    framework.accounting.types
    tests/finance_accounting/test_types.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-ledger-account-test
    framework.accounting.ledger.account
    tests/finance_accounting/test_ledger_account.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-chart-of-accounts-test
    framework.accounting.chart.of.accounts
    tests/finance_accounting/test_chart_of_accounts.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-account-hierarchy-test
    framework.accounting.account.hierarchy
    tests/finance_accounting/test_account_hierarchy.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-accounting-period-test
    framework.accounting.accounting.period
    tests/finance_accounting/test_accounting_period.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-accounting-calendar-test
    framework.accounting.accounting.calendar
    tests/finance_accounting/test_accounting_calendar.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-accounting-event-test
    framework.accounting.accounting.event
    tests/finance_accounting/test_accounting_event.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-posting-rule-test
    framework.accounting.posting.rule
    tests/finance_accounting/test_posting_rule.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-journal-line-test
    framework.accounting.journal.line
    tests/finance_accounting/test_journal_line.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-journal-entry-test
    framework.accounting.journal.entry
    tests/finance_accounting/test_journal_entry.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-journal-book-test
    framework.accounting.journal.book
    tests/finance_accounting/test_journal_book.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-double-entry-validation-test
    framework.accounting.double.entry.validation
    tests/finance_accounting/test_double_entry_validation.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-posting-engine-test
    framework.accounting.posting.engine
    tests/finance_accounting/test_posting_engine.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-ledger-balance-test
    framework.accounting.ledger.balance
    tests/finance_accounting/test_ledger_balance.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-trial-balance-test
    framework.accounting.trial.balance
    tests/finance_accounting/test_trial_balance.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-subledger-test
    framework.accounting.subledger
    tests/finance_accounting/test_subledger.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-reconciliation-rule-test
    framework.accounting.reconciliation.rule
    tests/finance_accounting/test_reconciliation_rule.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-reconciliation-item-test
    framework.accounting.reconciliation.item
    tests/finance_accounting/test_reconciliation_item.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-reconciliation-run-test
    framework.accounting.reconciliation.run
    tests/finance_accounting/test_reconciliation_run.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-suspense-account-test
    framework.accounting.suspense.account
    tests/finance_accounting/test_suspense_account.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-financial-control-test
    framework.accounting.financial.control
    tests/finance_accounting/test_financial_control.c
    accounting-core
)
umicom_add_banking_payments_accounting_test(
    umicom-accounting-accounting-service-test
    framework.accounting.accounting.service
    tests/finance_accounting/test_accounting_service.c
    accounting-core
)
endif()
message(STATUS "Umicom Framework banking, payments and accounting cores enabled")

if(BUILD_TESTING)
    add_executable(umi-accounting-journal-boundaries-test "${CMAKE_CURRENT_LIST_DIR}/../tests/finance_accounting/test_journal_boundaries.c")
    target_link_libraries(umi-accounting-journal-boundaries-test PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-journal-boundaries-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-journal-boundaries-test)
    umicom_apply_sanitizers(umi-accounting-journal-boundaries-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-journal-boundaries-test)
    endif()
    foreach(case normal empty named-statuses invalid-count invalid-date invalid-status negative-status invalid-id unterminated-id invalid-line-id invalid-account unterminated-account negative-debit negative-credit both-sides zero-sides debit-overflow credit-overflow both-overflow maximum null-totals same-output null-entry signed-credit signed-invalid signed-null line-init-atomic line-init-alias entry-init-atomic entry-init-alias append-overflow append-invalid-existing append-readonly append-full append-alias book-corrupt-count book-corrupt-entry book-duplicate book-post-overflow book-full book-null)
        set(test framework.accounting.journal.boundaries.${case})
        add_test(NAME ${test} COMMAND umi-accounting-journal-boundaries-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;accounting;boundaries;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-journal-boundaries-test)
        endif()
    endforeach()
endif()

if(BUILD_TESTING)
    add_executable(umi-accounting-posting-boundaries-test "${CMAKE_CURRENT_LIST_DIR}/../tests/finance_accounting/test_posting_boundaries.c")
    target_link_libraries(umi-accounting-posting-boundaries-test PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-posting-boundaries-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-posting-boundaries-test)
    umicom_apply_sanitizers(umi-accounting-posting-boundaries-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-posting-boundaries-test)
    endif()
    foreach(case positive negative maximum minimum event-id event-type event-date event-currency event-zero event-type-unterminated event-id-unterminated event-currency-unterminated rule-id rule-account rule-type-unterminated rule-account-unterminated rule-inactive rule-mismatch journal-id null-event null-rule null-output null-engine generated-exhausted rejected-exhausted event-init-atomic event-init-alias magnitude-minimum magnitude-negative magnitude-null)
        set(test framework.accounting.posting.boundaries.${case})
        add_test(NAME ${test} COMMAND umi-accounting-posting-boundaries-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;accounting;boundaries;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-posting-boundaries-test)
        endif()
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/accounting-journal-boundaries.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/guides COMPONENT Learning)

if(BUILD_TESTING)
    add_executable(umi-accounting-control-boundaries-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/finance_accounting/test_control_boundaries.c")
    target_link_libraries(umi-accounting-control-boundaries-test PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-control-boundaries-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-control-boundaries-test)
    umicom_apply_sanitizers(umi-accounting-control-boundaries-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-control-boundaries-test)
    endif()
    foreach(case ledger-debit ledger-credit ledger-zero ledger-negative ledger-side ledger-id ledger-account ledger-null ledger-output-null ledger-init-atomic ledger-init-alias trial-negative trial-id trial-zero trial-unequal trial-init-atomic double-negative double-id double-count double-zero double-unequal double-init-atomic)
        set(test framework.accounting.control.boundaries.${case})
        add_test(NAME ${test} COMMAND umi-accounting-control-boundaries-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;accounting;boundaries;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-control-boundaries-test)
        endif()
    endforeach()
endif()


if(BUILD_TESTING)
    add_executable(umi-accounting-catalogue-boundaries-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/finance_accounting/test_account_catalogue_boundaries.c")
    target_link_libraries(umi-accounting-catalogue-boundaries-test PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-catalogue-boundaries-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-catalogue-boundaries-test)
    umicom_apply_sanitizers(umi-accounting-catalogue-boundaries-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-catalogue-boundaries-test)
    endif()
    foreach(case named-classes blocked empty-id archive-invalid-id id-unterminated name-unterminated empty-name invalid-class negative-class invalid-side null init-atomic init-alias chart-find chart-null chart-null-key chart-count chart-invalid-item chart-restored-duplicate chart-invalid-tail chart-full chart-duplicate chart-alias chart-null-item)
        set(test framework.accounting.catalogue.boundaries.${case})
        add_test(NAME ${test} COMMAND umi-accounting-catalogue-boundaries-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;accounting;boundaries;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-catalogue-boundaries-test)
        endif()
    endforeach()
endif()

if(BUILD_TESTING)
    add_executable(umi-accounting-period-boundaries-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/finance_accounting/test_period_calendar_boundaries.c")
    target_link_libraries(umi-accounting-period-boundaries-test PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-period-boundaries-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-period-boundaries-test)
    umicom_apply_sanitizers(umi-accounting-period-boundaries-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-period-boundaries-test)
    endif()
    foreach(case inclusive closed soft-closed id id-unterminated reversed date status negative-status null init-alias init-atomic calendar-find calendar-null calendar-count calendar-invalid-period calendar-restored-overlap calendar-restored-id calendar-duplicate-id calendar-overlap calendar-full calendar-null-period calendar-alias)
        set(test framework.accounting.period.boundaries.${case})
        add_test(NAME ${test} COMMAND umi-accounting-period-boundaries-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;accounting;boundaries;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-period-boundaries-test)
        endif()
    endforeach()
endif()


# Share local posting inspection with all finance consumers; the API performs
# no provider calls and remains usable in the portable installed SDK.
target_sources(umicom_finance PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/finance/accounting/posting_review.c")
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_finance
        include/umicom/finance/accounting/posting_review.h)
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/finance/accounting/posting_review.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/finance/accounting COMPONENT Framework)


if(BUILD_TESTING)
    add_executable(umi-accounting-posting-review-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/finance_accounting/test_posting_review.c")
    target_link_libraries(umi-accounting-posting-review-test PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-posting-review-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-posting-review-test)
    umicom_apply_sanitizers(umi-accounting-posting-review-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-posting-review-test)
    endif()
    foreach(case inspect-ready inspect-invalid-entry inspect-posted inspect-reversed inspect-overflow inspect-unbalanced inspect-empty inspect-chart inspect-calendar inspect-book inspect-missing-period inspect-closed inspect-soft-closed inspect-missing-account inspect-blocked-account inspect-duplicate inspect-full inspect-null-entry inspect-null-chart inspect-null-calendar inspect-null-book inspect-null-output null-accessors apply tails entry-amount entry-date entry-state entry-id line-id line-order chart-name chart-class chart-side chart-blocked chart-invalid period-id period-range period-closed period-order period-invalid book-amount book-id book-invalid book-append different-entry different-chart different-calendar different-book null-review null-entry null-chart null-calendar null-book)
        set(test framework.accounting.posting.review.${case})
        add_test(NAME ${test} COMMAND umi-accounting-posting-review-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;accounting;review;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-posting-review-test)
        endif()
    endforeach()
endif()


install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/journal_posting/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/journal_posting/CMakeLists.txt"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/journal_posting COMPONENT Learning)
if(BUILD_TESTING)
    add_executable(umi-journal-posting-consumer "${CMAKE_CURRENT_LIST_DIR}/../examples/journal_posting/main.c")
    target_link_libraries(umi-journal-posting-consumer PRIVATE Umicom::finance)
    set_target_properties(umi-journal-posting-consumer PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-journal-posting-consumer)
    umicom_apply_sanitizers(umi-journal-posting-consumer)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-journal-posting-consumer)
    endif()
    foreach(scenario apply stale-account closed-period amount-boundary stale-book)
        set(test framework.accounting.journal.consumer.${scenario})
        add_test(NAME ${test} COMMAND umi-journal-posting-consumer ${scenario})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;accounting;consumer;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-journal-posting-consumer)
        endif()
    endforeach()
endif()


install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/journal-posting-review.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/guides COMPONENT Learning)


if(BUILD_TESTING)
    add_executable(umi-accounting-reconciliation-arithmetic-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/finance_accounting/test_reconciliation_arithmetic.c")
    target_link_libraries(umi-accounting-reconciliation-arithmetic-test PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-reconciliation-arithmetic-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-reconciliation-arithmetic-test)
    umicom_apply_sanitizers(umi-accounting-reconciliation-arithmetic-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-reconciliation-arithmetic-test)
    endif()
    foreach(case equal equal-minimum equal-maximum positive negative negative-values outside opposite-signs maximum-distance reverse-maximum-distance minimum-vs-zero zero-vs-minimum maximum-vs-zero zero-vs-maximum positive-overflow negative-overflow minimum-neighbour maximum-neighbour invalid-tolerance invalid-id unterminated-id null-output null init-atomic init-alias subledger-positive subledger-negative subledger-zero subledger-bad-debit subledger-bad-credit subledger-id subledger-control subledger-init-atomic subledger-init-alias subledger-null subledger-output-null)
        set(test framework.accounting.reconciliation.arithmetic.${case})
        add_test(NAME ${test} COMMAND umi-accounting-reconciliation-arithmetic-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;accounting;reconciliation;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-reconciliation-arithmetic-test)
        endif()
    endforeach()
endif()
