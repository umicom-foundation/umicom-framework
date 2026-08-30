/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/physical_exposure.h
 *
 * PURPOSE:
 *   Aggregate committed purchase and sale quantities into net physical exposure.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_PHYSICAL_EXPOSURE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_PHYSICAL_EXPOSURE_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityPhysicalExposure {
    UmiCommodityId commodity_id;
    int64_t purchase_units;
    int64_t sale_units;
    int32_t scale;
    char unit_code[UMI_COMMODITY_CODE_CAPACITY];
} UmiCommodityPhysicalExposure;

/* Initialise exposure for one commodity and unit basis. */
UmiStatus umi_commodity_physical_exposure_init(UmiCommodityPhysicalExposure *value, const char *commodity_id, int32_t scale, const char *unit_code);

/* Accumulate signed commercial commitments without mixing unit bases. */
UmiStatus umi_commodity_physical_exposure_add(UmiCommodityPhysicalExposure *value, int64_t purchase_units, int64_t sale_units);

/* Return purchases minus sales as the net physical exposure. */
int64_t umi_commodity_physical_exposure_net(const UmiCommodityPhysicalExposure *value);

#ifdef __cplusplus
}
#endif

#endif
