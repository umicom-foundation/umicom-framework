/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/schedule_rule.h
 *
 * PURPOSE:
 *   Define reusable generation rules for accrual and payment schedules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_SCHEDULE_RULE_H
#define UMICOM_FINANCE_CORE_SCHEDULE_RULE_H

#include "umicom/finance/core/tenor.h"
#include "umicom/finance/core/business_day_convention.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiScheduleRule { UmiFinancialDate start; UmiFinancialDate end; UmiTenor frequency; UmiBusinessDayConvention convention; } UmiScheduleRule;
/* Validate a schedule rule. */ bool umi_schedule_rule_is_valid(const UmiScheduleRule *r);
#ifdef __cplusplus
}
#endif

#endif
