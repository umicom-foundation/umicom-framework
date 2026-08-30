/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/cashflow_schedule.h
 *
 * PURPOSE:
 *   Provide bounded cashflow schedules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_CASHFLOW_SCHEDULE_H
#define UMICOM_FINANCE_CORE_CASHFLOW_SCHEDULE_H

#include "umicom/finance/core/cashflow.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCashflowSchedule { UmiCashflow items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiCashflowSchedule;
/* Reset collection. */ void umi_cashflow_schedule_init(UmiCashflowSchedule *c);
/* Append valid item. */ UmiStatus umi_cashflow_schedule_add(UmiCashflowSchedule *c,const UmiCashflow *item);
/* Return item count. */ size_t umi_cashflow_schedule_count(const UmiCashflowSchedule *c);
#ifdef __cplusplus
}
#endif

#endif
