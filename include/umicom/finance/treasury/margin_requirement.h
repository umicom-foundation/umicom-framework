/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/margin_requirement.h
 *
 * PURPOSE:
 *   Calculate required margin after threshold and independent amount.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_MARGIN_REQUIREMENT_H
#define UMICOM_FINANCE_TREASURY_MARGIN_REQUIREMENT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryMarginRequirement {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t exposure_minor;
    int64_t threshold_minor;
    int64_t independent_amount_minor;
} UmiTreasuryMarginRequirement;
UmiStatus umi_treasury_margin_requirement_init(UmiTreasuryMarginRequirement *value,
    const char *id,
    int64_t exposure_minor,
    int64_t threshold_minor,
    int64_t independent_amount_minor);
bool umi_treasury_margin_requirement_valid(const UmiTreasuryMarginRequirement *value);
int64_t umi_treasury_margin_requirement_required_minor(const UmiTreasuryMarginRequirement *value);
#ifdef __cplusplus
}
#endif
#endif
