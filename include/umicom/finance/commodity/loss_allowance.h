/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/loss_allowance.h
 *
 * PURPOSE:
 *   Define permitted physical loss as basis points of shipped quantity.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_LOSS_ALLOWANCE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_LOSS_ALLOWANCE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity loss allowance data shared with callers of this public contract.
 */
typedef struct UmiCommodityLossAllowance {
    UmiCommodityId contract_id;
    int32_t basis_points;
    bool active;
} UmiCommodityLossAllowance;

/* Initialise a bounded loss allowance record for reusable Framework workflows. */
UmiStatus umi_commodity_loss_allowance_init(UmiCommodityLossAllowance *value, const char *contract_id, int32_t basis_points);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_loss_allowance_valid(const UmiCommodityLossAllowance *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_commodity_loss_allowance_archive_encode(const UmiCommodityLossAllowance *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_commodity_loss_allowance_archive_decode(const void *bytes, size_t byte_count,
    UmiCommodityLossAllowance *value);

#ifdef __cplusplus
}
#endif

#endif
