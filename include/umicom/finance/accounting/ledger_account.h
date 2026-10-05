/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/ledger_account.h
 *
 * PURPOSE:
 *   Represent general-ledger accounts, normal side and posting eligibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_LEDGER_ACCOUNT_H
#define UMICOM_FINANCE_ACCOUNTING_LEDGER_ACCOUNT_H
#include "umicom/finance/accounting/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting ledger account data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingLedgerAccount {
    UmiFinancialId id;
    char name[UMI_ACCOUNTING_NAME_CAPACITY];
    UmiAccountingAccountClass account_class;
    UmiAccountingNormalSide normal_side;
    bool posting_allowed;
} UmiAccountingLedgerAccount;
/**
 * Initialise accounting ledger account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_ledger_account_init(UmiAccountingLedgerAccount *value,
    const char *id,
    const char *name,
    UmiAccountingAccountClass account_class,
    UmiAccountingNormalSide normal_side,
    bool posting_allowed);
/**
 * Check that accounting ledger account satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_ledger_account_valid(const UmiAccountingLedgerAccount *value);
/**
 * Provide the accounting ledger account postable operation used by this module and its
 * client applications.
 */
bool umi_accounting_ledger_account_postable(const UmiAccountingLedgerAccount *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_accounting_ledger_account_archive_encode(const UmiAccountingLedgerAccount *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_accounting_ledger_account_archive_decode(const void *bytes, size_t byte_count,
    UmiAccountingLedgerAccount *value);

#ifdef __cplusplus
}
#endif
#endif
