/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_schedule_rule.c
 *
 * PURPOSE:
 *   Exercise the schedule rule financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/schedule_rule.h"

int main(void)
{
    UmiScheduleRule r={(UmiFinancialDate){2026,1U,1U},(UmiFinancialDate){2027,1U,1U},{3U,UMI_TENOR_MONTHS},UMI_BUSINESS_DAY_FOLLOWING}; CHECK(umi_schedule_rule_is_valid(&r));
    return 0;
}
