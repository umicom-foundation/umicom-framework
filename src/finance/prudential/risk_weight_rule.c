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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/risk_weight_rule.h"

#include <string.h>
UmiStatus umi_pru_risk_weight_rule_init(UmiPrudentialRiskWeightRule *rule,const char *exposure_class_id,double risk_weight) { UmiStatus s; if(rule==NULL||exposure_class_id==NULL||!umi_pru_number_valid(risk_weight)||risk_weight<0.0||risk_weight>12.5)return UMI_STATUS_INVALID_ARGUMENT; memset(rule,0,sizeof *rule); s=umi_pru_copy_text(rule->exposure_class_id,sizeof rule->exposure_class_id,exposure_class_id); if(s!=UMI_STATUS_OK)return s; rule->risk_weight=risk_weight; return UMI_STATUS_OK; }
