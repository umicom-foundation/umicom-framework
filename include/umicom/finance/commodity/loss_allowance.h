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

#ifdef __cplusplus
}
#endif

#endif
