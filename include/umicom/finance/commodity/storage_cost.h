/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/storage_cost.h
 *
 * PURPOSE:
 *   Calculate deterministic storage charges from daily rates and occupied units.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_STORAGE_COST_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_STORAGE_COST_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityStorageCost {
    UmiCommodityId facility_id;
    int64_t minor_units_per_day_per_unit;
    UmiCurrency currency;
} UmiCommodityStorageCost;

/* Initialise a non-negative storage tariff. */
UmiStatus umi_commodity_storage_cost_init(UmiCommodityStorageCost *value, const char *facility_id, int64_t minor_units_per_day_per_unit, const UmiCurrency *currency);

/* Calculate storage cost for whole units and days with overflow protection. */
UmiStatus umi_commodity_storage_cost_calculate(const UmiCommodityStorageCost *value, int64_t units, int32_t days, int64_t *out_minor_units);

#ifdef __cplusplus
}
#endif

#endif
