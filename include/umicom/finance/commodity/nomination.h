/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/nomination.h
 *
 * PURPOSE:
 *   Define a quantity nomination against a physical contract and delivery window.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_NOMINATION_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_NOMINATION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity nomination data shared with callers of this public contract.
 */
typedef struct UmiCommodityNomination {
    UmiCommodityId id;
    UmiCommodityId contract_id;
    UmiCommodityQuantity quantity;
    int64_t window_start_ms;
    int64_t window_end_ms;
    bool accepted;
} UmiCommodityNomination;

/* Initialise a bounded nomination record for reusable Framework workflows. */
UmiStatus umi_commodity_nomination_init(UmiCommodityNomination *value, const char *id, const char *contract_id, int64_t units, int32_t scale, const char *unit_code, int64_t window_start_ms, int64_t window_end_ms);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_nomination_valid(const UmiCommodityNomination *value);

#ifdef __cplusplus
}
#endif

#endif
