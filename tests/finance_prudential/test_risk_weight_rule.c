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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialRiskWeightRule r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_risk_weight_rule_init(&r,"CORP",1.0)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(r.risk_weight!=1.0)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_risk_weight_rule_init(&r,"BAD",13.0)!=UMI_STATUS_INVALID_ARGUMENT)return 3; return 0; }
