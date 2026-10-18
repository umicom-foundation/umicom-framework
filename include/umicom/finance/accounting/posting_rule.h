/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/posting_rule.h
 *
 * PURPOSE:
 *   Map canonical accounting event types to debit and credit ledger accounts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_POSTING_RULE_H
#define UMICOM_FINANCE_ACCOUNTING_POSTING_RULE_H
#include "umicom/finance/accounting/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting posting rule data shared with callers of this public contract.
 */
typedef struct UmiAccountingPostingRule {
    UmiFinancialId id;
    char event_type[UMI_FINANCIAL_CORE_CODE_CAPACITY];
    UmiFinancialId debit_account_id;
    UmiFinancialId credit_account_id;
    bool active;
} UmiAccountingPostingRule;
/**
 * Initialise accounting posting rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_posting_rule_init(UmiAccountingPostingRule *value,
    const char *id,
    const char *event_type,
    const char *debit_account_id,
    const char *credit_account_id,
    bool active);
/**
 * Check that accounting posting rule satisfies its contract before another service relies
 * on it.
 */
bool umi_accounting_posting_rule_valid(const UmiAccountingPostingRule *value);
/**
 * Provide the accounting posting rule usable operation used by this module and its client
 * applications.
 */
bool umi_accounting_posting_rule_usable(const UmiAccountingPostingRule *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_accounting_posting_rule_archive_encode(const UmiAccountingPostingRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_accounting_posting_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiAccountingPostingRule *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiAccountingPostingRule object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_accounting_posting_rule_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiAccountingPostingRule *workspace, UmiAccountingPostingRule *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_accounting_posting_rule_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAccountingPostingRule *expected, UmiAccountingPostingRule *value);


#ifdef __cplusplus
}
#endif
#endif
