/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_resource_budget.c
 *
 * PURPOSE:
 *   Regression coverage for bound files, lines, attempts and execution time consumed by one helix engineering session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/resource_budget.h"
int main(void){ UmiHelixResourceBudget r; umi_helix_resource_budget_init(&r,"resource_budget"); if(umi_helix_resource_budget_validate(&r)!=UMI_STATUS_OK){ return 1; } r.limit=10U; r.used=2U; double s=umi_helix_resource_budget_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
