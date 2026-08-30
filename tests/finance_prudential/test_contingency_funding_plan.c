/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_contingency_funding_plan.c
 *
 * PURPOSE:
 *   Verify store executable contingency funding actions and available capacity.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/contingency_funding_plan.h"

int main(void) { UmiPrudentialContingencyFundingPlan p={0}; if(umi_pru_contingency_funding_plan_add(&p,"A",1,10.0)!=UMI_STATUS_OK)return 1; if(umi_pru_contingency_funding_plan_add(&p,"B",2,20.0)!=UMI_STATUS_OK)return 2; return umi_pru_contingency_funding_plan_total(&p)==30.0?0:3; }
