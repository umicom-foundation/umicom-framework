/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/types.h
 *
 * PURPOSE:
 *   Define shared financial identifiers, dates, enums and bounded helper operations while reusing existing UmiCurrency/UmiMoney/UmiQuantity values.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_TYPES_H
#define UMICOM_FINANCE_CORE_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_FINANCIAL_CORE_NAME_CAPACITY 96U
#define UMI_FINANCIAL_CORE_CODE_CAPACITY 32U
#define UMI_FINANCIAL_CORE_MAX_ITEMS 64U
typedef struct UmiFinancialDate { int32_t year; uint8_t month; uint8_t day; } UmiFinancialDate;
typedef enum UmiFinancialDirection { UMI_FINANCIAL_DIRECTION_NONE=0, UMI_FINANCIAL_DIRECTION_PAY=1, UMI_FINANCIAL_DIRECTION_RECEIVE=2 } UmiFinancialDirection;
typedef enum UmiFinancialLifecycleState { UMI_FINANCIAL_LIFECYCLE_DRAFT=0, UMI_FINANCIAL_LIFECYCLE_ACTIVE=1, UMI_FINANCIAL_LIFECYCLE_MATURED=2, UMI_FINANCIAL_LIFECYCLE_CANCELLED=3 } UmiFinancialLifecycleState;
typedef enum UmiSettlementState { UMI_SETTLEMENT_PENDING=0, UMI_SETTLEMENT_MATCHED=1, UMI_SETTLEMENT_SETTLED=2, UMI_SETTLEMENT_FAILED=3, UMI_SETTLEMENT_CANCELLED=4 } UmiSettlementState;
typedef enum UmiProductClass { UMI_PRODUCT_UNKNOWN=0, UMI_PRODUCT_CASH=1, UMI_PRODUCT_FX=2, UMI_PRODUCT_FIXED_INCOME=3, UMI_PRODUCT_EQUITY=4, UMI_PRODUCT_DERIVATIVE=5, UMI_PRODUCT_COMMODITY=6, UMI_PRODUCT_DIGITAL_ASSET=7 } UmiProductClass;
typedef enum UmiDayCountConvention { UMI_DAY_COUNT_ACT_360=0, UMI_DAY_COUNT_ACT_365F=1, UMI_DAY_COUNT_THIRTY_360=2 } UmiDayCountConvention;
typedef enum UmiBusinessDayConvention { UMI_BUSINESS_DAY_FOLLOWING=0, UMI_BUSINESS_DAY_MODIFIED_FOLLOWING=1, UMI_BUSINESS_DAY_PRECEDING=2, UMI_BUSINESS_DAY_MODIFIED_PRECEDING=3, UMI_BUSINESS_DAY_UNADJUSTED=4 } UmiBusinessDayConvention;
typedef enum UmiTenorUnit { UMI_TENOR_DAYS=0, UMI_TENOR_WEEKS=1, UMI_TENOR_MONTHS=2, UMI_TENOR_YEARS=3 } UmiTenorUnit;
typedef enum UmiQuoteSide { UMI_QUOTE_MID=0, UMI_QUOTE_BID=1, UMI_QUOTE_ASK=2, UMI_QUOTE_LAST=3 } UmiQuoteSide;
/* Copy text into a bounded caller-owned buffer. */ UmiStatus umi_financial_core_copy(char *dst,size_t cap,const char *src);
/* Assign a reusable existing financial identifier. */ UmiStatus umi_financial_id_assign(UmiFinancialId *id,const char *value);
/* Test whether a reusable financial identifier is populated. */ bool umi_financial_id_is_valid(const UmiFinancialId *id);
/* Compare reusable financial identifiers lexically. */ int umi_financial_id_compare(const UmiFinancialId *a,const UmiFinancialId *b);
/* Validate a Gregorian financial date. */ bool umi_financial_date_is_valid(UmiFinancialDate d);
/* Compare two financial dates. */ int umi_financial_date_compare(UmiFinancialDate a,UmiFinancialDate b);
#ifdef __cplusplus
}
#endif

#endif
