/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/term_deposit.h
 *
 * PURPOSE:
 *   Represent principal, maturity and rollover intent for term deposits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_TERM_DEPOSIT_H
#define UMICOM_FINANCE_BANKING_TERM_DEPOSIT_H
#include "umicom/finance/banking/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking term deposit data shared with callers of this public contract.
 */
typedef struct UmiBankingTermDeposit {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    int64_t principal_minor;
    UmiFinancialDate start_date;
    UmiFinancialDate maturity_date;
    bool rollover;
} UmiBankingTermDeposit;
/**
 * Initialise banking term deposit from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_term_deposit_init(UmiBankingTermDeposit *value,
    const char *id,
    const char *customer_id,
    int64_t principal_minor,
    UmiFinancialDate start_date,
    UmiFinancialDate maturity_date,
    bool rollover);
/**
 * Check that banking term deposit satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_term_deposit_valid(const UmiBankingTermDeposit *value);
/**
 * Provide the banking term deposit auto rollover operation used by this module and its
 * client applications.
 */
bool umi_banking_term_deposit_auto_rollover(const UmiBankingTermDeposit *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_banking_term_deposit_archive_encode(const UmiBankingTermDeposit *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_banking_term_deposit_archive_decode(const void *bytes, size_t byte_count,
    UmiBankingTermDeposit *value);

#ifdef __cplusplus
}
#endif
#endif
