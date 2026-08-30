/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/liquidity_buffer.h
 *
 * PURPOSE:
 *   Compare available liquidity against a policy buffer requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIQUIDITY_BUFFER_H
#define UMICOM_FINANCE_TREASURY_LIQUIDITY_BUFFER_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryLiquidityBuffer {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t available_minor;
    int64_t required_minor;
} UmiTreasuryLiquidityBuffer;
UmiStatus umi_treasury_liquidity_buffer_init(UmiTreasuryLiquidityBuffer *value,
    const char *id,
    int64_t available_minor,
    int64_t required_minor);
bool umi_treasury_liquidity_buffer_valid(const UmiTreasuryLiquidityBuffer *value);
int64_t umi_treasury_liquidity_buffer_surplus_minor(const UmiTreasuryLiquidityBuffer *value);
#ifdef __cplusplus
}
#endif
#endif
