/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/scenario_result.c
 *
 * PURPOSE:
 *   Capture portfolio P&L and derived non-negative loss for one scenario.
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

#include "umicom/finance/enterprise/scenario_result.h"

#include <string.h>
/*
 * Initialise enterprise scenario result from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_scenario_result_init(UmiEnterpriseScenarioResult *r,const char *sid,const char *pid,double pnl){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||!umi_quant_number_valid(pnl))return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof *r);s=umi_quant_copy_text(r->scenario_id,sizeof r->scenario_id,sid);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_quant_copy_text(r->portfolio_id,sizeof r->portfolio_id,pid);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;r->pnl=pnl;r->loss=(pnl<0.0)?-pnl:0.0;return UMI_STATUS_OK;}
