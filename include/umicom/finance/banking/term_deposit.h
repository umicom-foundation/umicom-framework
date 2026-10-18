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

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiBankingTermDeposit object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_banking_term_deposit_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiBankingTermDeposit *workspace, UmiBankingTermDeposit *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_banking_term_deposit_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiBankingTermDeposit *expected, UmiBankingTermDeposit *value);


#ifdef __cplusplus
}
#endif
#endif
