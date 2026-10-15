/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/balance.h
 *
 * PURPOSE:
 *   Represent dated financial balances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_BALANCE_H
#define UMICOM_FINANCE_CORE_BALANCE_H

#include "umicom/finance/core/business_date.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the financial balance data shared with callers of this public contract.
 */
typedef struct UmiFinancialBalance { UmiFinancialId id; UmiMoney amount; UmiFinancialDate date; uint32_t state; } UmiFinancialBalance;
/* Initialize monetary record. */ UmiStatus umi_balance_init(UmiFinancialBalance *x,const char *id,UmiMoney amount,UmiFinancialDate date,uint32_t state);
/* Validate monetary record. */ bool umi_balance_is_valid(const UmiFinancialBalance *x);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_balance_archive_encode(const UmiFinancialBalance *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_balance_archive_decode(const void *bytes, size_t byte_count,
    UmiFinancialBalance *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_balance_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFinancialBalance *expected, UmiFinancialBalance *value);


#ifdef __cplusplus
}
#endif

#endif
