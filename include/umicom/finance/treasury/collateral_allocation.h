/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/collateral_allocation.h
 *
 * PURPOSE:
 *   Represent collateral allocation against a secured requirement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_COLLATERAL_ALLOCATION_H
#define UMICOM_FINANCE_TREASURY_COLLATERAL_ALLOCATION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryCollateralAllocation {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t required_minor;
    int64_t allocated_minor;
} UmiTreasuryCollateralAllocation;
UmiStatus umi_treasury_collateral_allocation_init(UmiTreasuryCollateralAllocation *value,
    const char *id,
    int64_t required_minor,
    int64_t allocated_minor);
bool umi_treasury_collateral_allocation_valid(const UmiTreasuryCollateralAllocation *value);
int64_t umi_treasury_collateral_allocation_shortfall_minor(const UmiTreasuryCollateralAllocation *value);
#ifdef __cplusplus
}
#endif
#endif
