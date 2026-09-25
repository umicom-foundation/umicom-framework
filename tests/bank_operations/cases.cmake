# Umicom Framework - banking operations regression case catalogue.
# Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
set(UMICOM_BANK_OPERATIONS_TEST_CASES
    unused-command-field recipient-overflow sqlite-rollback-poison
    end-to-end idempotent idempotency-conflict idempotency-actor idempotency-permission
 stale-revision insufficient-funds reservation-double-spend self-approval unapproved-execution
 rejection cancellation double-execution reversal reversal-reserved currency-mismatch currency-scales
 account-block customer-block beneficiary-block beneficiary-owner account-close close-pending-recipient
 manual-hold hold-overflow credit-overflow credit-permission statement-range reconciliation-break copy-isolation audit-lineage
 card-limit card-freeze card-void card-close-reserved card-overcapture card-manual-release card-capture card-refund
 invalid-id reserved-id invalid-date negative-money invalid-name invalid-state invalid-currency invalid-scale
 customer-capacity event-capacity codec-roundtrip codec-truncation codec-version codec-trailing absolute-path
 sqlite-restart sqlite-stale-writer sqlite-write-failure sqlite-commit-failure sqlite-corrupt-event
 sqlite-missing-event sqlite-orphan-event sqlite-unknown-schema sqlite-read-error sqlite-enumeration-error sqlite-embedded-nul)
