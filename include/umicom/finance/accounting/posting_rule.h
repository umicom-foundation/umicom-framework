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

#ifdef __cplusplus
}
#endif
#endif
