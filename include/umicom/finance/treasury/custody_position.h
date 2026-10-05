/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/custody_position.h
 *
 * PURPOSE:
 *   Represent settled, pending-in and pending-out custody quantities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CUSTODY_POSITION_H
#define UMICOM_FINANCE_TREASURY_CUSTODY_POSITION_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury custody position data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryCustodyPosition {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t settled_quantity;
    int64_t pending_in_quantity;
    int64_t pending_out_quantity;
} UmiTreasuryCustodyPosition;
/**
 * Initialise treasury custody position from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_custody_position_init(UmiTreasuryCustodyPosition *value,
    const char *id,
    int64_t settled_quantity,
    int64_t pending_in_quantity,
    int64_t pending_out_quantity);
/**
 * Check that treasury custody position satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_custody_position_valid(const UmiTreasuryCustodyPosition *value);
/**
 * Provide the treasury custody position projected quantity operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_custody_position_projected_quantity(const UmiTreasuryCustodyPosition *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_custody_position_archive_encode(const UmiTreasuryCustodyPosition *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_custody_position_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryCustodyPosition *value);

#ifdef __cplusplus
}
#endif
#endif
