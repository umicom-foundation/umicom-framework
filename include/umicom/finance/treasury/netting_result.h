/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/netting_result.h
 *
 * PURPOSE:
 *   Record gross and net exposure reduction from netting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_NETTING_RESULT_H
#define UMICOM_FINANCE_TREASURY_NETTING_RESULT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury netting result data shared with callers of this public contract.
 */
typedef struct UmiTreasuryNettingResult {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t gross_minor;
    int64_t net_minor;
} UmiTreasuryNettingResult;
/**
 * Initialise treasury netting result from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_netting_result_init(UmiTreasuryNettingResult *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor);
/**
 * Check that treasury netting result satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_netting_result_valid(const UmiTreasuryNettingResult *value);
/**
 * Provide the treasury netting result reduction minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_netting_result_reduction_minor(const UmiTreasuryNettingResult *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_netting_result_archive_encode(const UmiTreasuryNettingResult *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_netting_result_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryNettingResult *value);

#ifdef __cplusplus
}
#endif
#endif
