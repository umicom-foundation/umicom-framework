/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/schedule_rule.c
 *
 * PURPOSE:
 *   Implement schedule rule validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/schedule_rule.h"

/* Validate a schedule rule. */ bool umi_schedule_rule_is_valid(const UmiScheduleRule *r){return r!=NULL&&umi_financial_date_is_valid(r->start)&&umi_financial_date_is_valid(r->end)&&umi_financial_date_compare(r->start,r->end)<0&&r->frequency.amount>0U&&r->frequency.unit<=UMI_TENOR_YEARS&&r->convention<=UMI_BUSINESS_DAY_UNADJUSTED;}
