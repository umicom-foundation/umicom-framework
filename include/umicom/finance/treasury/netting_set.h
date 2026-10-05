/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/netting_set.h
 *
 * PURPOSE:
 *   Define gross receivables and payables within a legally enforceable netting set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_NETTING_SET_H
#define UMICOM_FINANCE_TREASURY_NETTING_SET_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury netting set data shared with callers of this public contract.
 */
typedef struct UmiTreasuryNettingSet {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t receivable_minor;
    int64_t payable_minor;
} UmiTreasuryNettingSet;
/**
 * Initialise treasury netting set from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_netting_set_init(UmiTreasuryNettingSet *value,
    const char *id,
    int64_t receivable_minor,
    int64_t payable_minor);
/**
 * Check that treasury netting set satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_netting_set_valid(const UmiTreasuryNettingSet *value);
/**
 * Provide the treasury netting set net minor operation used by this module and its client
 * applications.
 */
int64_t umi_treasury_netting_set_net_minor(const UmiTreasuryNettingSet *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_netting_set_archive_encode(const UmiTreasuryNettingSet *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_netting_set_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryNettingSet *value);

#ifdef __cplusplus
}
#endif
#endif
