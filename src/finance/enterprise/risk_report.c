/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/risk_report.c
 *
 * PURPOSE:
 *   Produce one auditable portfolio risk report from primary enterprise measures.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/risk_report.h"

#include <string.h>
UmiStatus umi_enterprise_risk_report_init(UmiEnterpriseRiskReport *r,const char *id,int64_t asof,double var,double es,double stress,double pnl){UmiStatus s;if(r==NULL||asof<0||!umi_quant_number_valid(var)||!umi_quant_number_valid(es)||!umi_quant_number_valid(stress)||!umi_quant_number_valid(pnl)||var<0.0||es<var||stress<0.0)return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof *r);s=umi_quant_copy_text(r->portfolio_id,sizeof r->portfolio_id,id);if(s!=UMI_STATUS_OK)return s;r->as_of_ms=asof;r->var=var;r->expected_shortfall=es;r->stress_loss=stress;r->pnl=pnl;return UMI_STATUS_OK;}
