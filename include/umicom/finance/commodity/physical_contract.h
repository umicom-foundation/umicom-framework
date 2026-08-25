/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/physical_contract.h
 *
 * PURPOSE:
 *   Define a reusable bilateral physical commodity contract with quantity and delivery terms.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_PHYSICAL_CONTRACT_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_PHYSICAL_CONTRACT_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityPhysicalContract {
    UmiCommodityId id;
    UmiCommodityId commodity_id;
    UmiFinancialId buyer_party_id;
    UmiFinancialId seller_party_id;
    UmiCommodityQuantity quantity;
    UmiCurrency price_currency;
    int64_t price_minor_units_per_unit;
    int64_t delivery_start_ms;
    int64_t delivery_end_ms;
    bool active;
} UmiCommodityPhysicalContract;

/* Initialise a bounded physical contract record for reusable Framework workflows. */
UmiStatus umi_commodity_physical_contract_init(UmiCommodityPhysicalContract *value, const char *id, const char *commodity_id, const UmiFinancialId *buyer_party_id, const UmiFinancialId *seller_party_id, int64_t units, int32_t scale, const char *unit_code, const UmiCurrency *price_currency, int64_t price_minor_units_per_unit, int64_t delivery_start_ms, int64_t delivery_end_ms);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_physical_contract_valid(const UmiCommodityPhysicalContract *value);

#ifdef __cplusplus
}
#endif

#endif
