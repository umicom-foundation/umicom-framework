/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/haircut_rule.c
 *
 * PURPOSE:
 *   Implement define collateral valuation haircut in basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/haircut_rule.h"
#include <string.h>
UmiStatus umi_treasury_haircut_rule_init(UmiTreasuryHaircutRule *value,
    const char *id,
    uint32_t haircut_bps) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->haircut_bps=haircut_bps;
    return umi_treasury_haircut_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_haircut_rule_valid(const UmiTreasuryHaircutRule *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->haircut_bps <= 10000U);
}

uint32_t umi_treasury_haircut_rule_remaining_bps(const UmiTreasuryHaircutRule *value) {
    if (value == NULL) return (uint32_t)0;
    return 10000U - value->haircut_bps;
}
