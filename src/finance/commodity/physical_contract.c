/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/physical_contract.c
 *
 * PURPOSE:
 *   Implement a reusable bilateral physical commodity contract with quantity and delivery terms.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/physical_contract.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_physical_contract_init(UmiCommodityPhysicalContract *value, const char *id, const char *commodity_id, const UmiFinancialId *buyer_party_id, const UmiFinancialId *seller_party_id, int64_t units, int32_t scale, const char *unit_code, const UmiCurrency *price_currency, int64_t price_minor_units_per_unit, int64_t delivery_start_ms, int64_t delivery_end_ms)
{
    UmiStatus status;
    if (value == NULL || buyer_party_id == NULL || seller_party_id == NULL || price_currency == NULL || units <= 0 || scale < 0 || price_minor_units_per_unit < 0 || delivery_start_ms < 0 || delivery_end_ms <= delivery_start_ms) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->commodity_id.value, sizeof value->commodity_id.value, commodity_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->quantity.unit_code, sizeof value->quantity.unit_code, unit_code);
    if (status != UMI_STATUS_OK) return status;
    value->buyer_party_id = *buyer_party_id;
    value->seller_party_id = *seller_party_id;
    value->quantity.units = units;
    value->quantity.scale = scale;
    value->price_currency = *price_currency;
    value->price_minor_units_per_unit = price_minor_units_per_unit;
    value->delivery_start_ms = delivery_start_ms;
    value->delivery_end_ms = delivery_end_ms;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_physical_contract_valid(const UmiCommodityPhysicalContract *value)
{
    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->commodity_id.value) && value->quantity.units > 0 && value->delivery_end_ms > value->delivery_start_ms && value->active);
}
