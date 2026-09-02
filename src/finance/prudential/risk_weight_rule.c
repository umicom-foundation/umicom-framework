/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/risk_weight_rule.c
 *
 * PURPOSE:
 *   Define a bounded risk-weight rule for an exposure class.
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

#include <string.h>
/*
 * Initialise pru risk weight rule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_pru_risk_weight_rule_init(UmiPrudentialRiskWeightRule *rule,const char *exposure_class_id,double risk_weight) { UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(rule==NULL||exposure_class_id==NULL||!umi_pru_number_valid(risk_weight)||risk_weight<0.0||risk_weight>12.5)return UMI_STATUS_INVALID_ARGUMENT; memset(rule,0,sizeof *rule); s=umi_pru_copy_text(rule->exposure_class_id,sizeof rule->exposure_class_id,exposure_class_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; rule->risk_weight=risk_weight; return UMI_STATUS_OK; }
