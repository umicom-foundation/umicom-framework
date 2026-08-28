/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/curve_reference.h
 *
 * PURPOSE:
 *   Define provider-neutral curve references.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_CURVE_REFERENCE_H
#define UMICOM_FINANCE_CORE_CURVE_REFERENCE_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCurveReference { UmiFinancialId reference_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; bool active; } UmiCurveReference;
/* Initialize the typed financial record. */ UmiStatus umi_curve_reference_init(UmiCurveReference *item,const char *id,const char *name,const char *code);
/* Validate the typed financial record. */ bool umi_curve_reference_is_valid(const UmiCurveReference *item);
#ifdef __cplusplus
}
#endif

#endif
