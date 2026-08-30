/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_risk_weight_rule.c
 *
 * PURPOSE:
 *   Verify define a bounded risk-weight rule for an exposure class.
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

#include "umicom/finance/prudential/risk_weight_rule.h"

int main(void) { UmiPrudentialRiskWeightRule r; if(umi_pru_risk_weight_rule_init(&r,"CORP",1.0)!=UMI_STATUS_OK)return 1; if(r.risk_weight!=1.0)return 2; if(umi_pru_risk_weight_rule_init(&r,"BAD",13.0)!=UMI_STATUS_INVALID_ARGUMENT)return 3; return 0; }
