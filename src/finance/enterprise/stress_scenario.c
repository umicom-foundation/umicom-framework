/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/stress_scenario.c
 *
 * PURPOSE:
 *   Represent a named severity-scaled stress factor shock.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/stress_scenario.h"

#include <string.h>
/*
 * Initialise enterprise stress scenario from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_stress_scenario_init(UmiEnterpriseStressScenario *s,const char *id,const char *factor,double shock,int severity){UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_quant_number_valid(shock)||severity<1||severity>5)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);st=umi_quant_copy_text(s->scenario_id,sizeof s->scenario_id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;st=umi_quant_copy_text(s->factor_name,sizeof s->factor_name,factor);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;s->shock=shock;s->severity=severity;return UMI_STATUS_OK;}
