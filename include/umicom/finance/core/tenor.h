/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/tenor.h
 *
 * PURPOSE:
 *   Parse and apply common day/week/month/year tenors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_TENOR_H
#define UMICOM_FINANCE_CORE_TENOR_H

#include "umicom/finance/core/business_date.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTenor { uint32_t amount; UmiTenorUnit unit; } UmiTenor;
/* Parse compact tenor. */ UmiStatus umi_tenor_parse(const char *text,UmiTenor *out);
/* Add tenor with month-end clamping. */ UmiStatus umi_tenor_add(UmiFinancialDate d,UmiTenor t,UmiFinancialDate *out);
#ifdef __cplusplus
}
#endif

#endif
