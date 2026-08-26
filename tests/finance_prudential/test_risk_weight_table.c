/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_risk_weight_table.c
 *
 * PURPOSE:
 *   Verify store and resolve ordered risk-weight rules by exposure-class identifier.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/risk_weight_table.h"

int main(void) { UmiPrudentialRiskWeightTable t={0}; UmiPrudentialRiskWeightRule r; double w=0.0; if(umi_pru_risk_weight_rule_init(&r,"BANK",0.5)!=UMI_STATUS_OK)return 1; if(umi_pru_risk_weight_table_add(&t,&r)!=UMI_STATUS_OK)return 2; if(umi_pru_risk_weight_table_find(&t,"BANK",&w)!=UMI_STATUS_OK)return 3; return w==0.5?0:4; }
