/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/types.h
 *
 * PURPOSE:
 *   Define shared bounded treasury, risk, settlement and collateral types used by reusable financial services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_TYPES_H
#define UMICOM_FINANCE_TREASURY_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TREASURY_ID_CAPACITY 64U
#define UMI_TREASURY_NAME_CAPACITY 96U
#define UMI_TREASURY_MAX_ITEMS 64U
#define UMI_TREASURY_MAX_BUCKETS 32U

typedef enum UmiTreasuryDirection { UMI_TREASURY_INFLOW=1, UMI_TREASURY_OUTFLOW=-1 } UmiTreasuryDirection;
typedef enum UmiTreasurySettlementState { UMI_TREASURY_SETTLEMENT_NEW=0, UMI_TREASURY_SETTLEMENT_MATCHED=1, UMI_TREASURY_SETTLEMENT_INSTRUCTED=2, UMI_TREASURY_SETTLEMENT_SETTLED=3, UMI_TREASURY_SETTLEMENT_FAILED=4, UMI_TREASURY_SETTLEMENT_CANCELLED=5 } UmiTreasurySettlementState;
typedef enum UmiTreasuryRiskClass { UMI_TREASURY_RISK_MARKET=1, UMI_TREASURY_RISK_CREDIT=2, UMI_TREASURY_RISK_LIQUIDITY=3, UMI_TREASURY_RISK_COUNTERPARTY=4 } UmiTreasuryRiskClass;
typedef enum UmiTreasuryCollateralKind { UMI_TREASURY_COLLATERAL_CASH=1, UMI_TREASURY_COLLATERAL_SECURITY=2 } UmiTreasuryCollateralKind;
typedef enum UmiTreasuryMarginState { UMI_TREASURY_MARGIN_OPEN=0, UMI_TREASURY_MARGIN_AGREED=1, UMI_TREASURY_MARGIN_DISPUTED=2, UMI_TREASURY_MARGIN_SETTLED=3 } UmiTreasuryMarginState;

bool umi_treasury_id_valid(const char *value);
UmiStatus umi_treasury_id_copy(char *destination, size_t capacity, const char *value);
UmiStatus umi_treasury_currency_from_code(const char *code, UmiCurrency *out_currency);
bool umi_treasury_currency_equal(UmiCurrency left, UmiCurrency right);
int64_t umi_treasury_abs_i64(int64_t value);
#ifdef __cplusplus
}
#endif
#endif
