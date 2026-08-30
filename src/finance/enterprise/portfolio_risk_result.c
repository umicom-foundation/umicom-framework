/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/portfolio_risk_result.c
 *
 * PURPOSE:
 *   Capture portfolio VaR, expected shortfall and stress loss in one reusable record.
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

#include "umicom/finance/enterprise/portfolio_risk_result.h"

#include <string.h>
UmiStatus umi_enterprise_portfolio_risk_result_init(UmiEnterprisePortfolioRiskResult *r,const char *id,double var,double es,double stress){ UmiStatus s; if(r==NULL||!umi_quant_number_valid(var)||!umi_quant_number_valid(es)||!umi_quant_number_valid(stress)||var<0.0||es<var||stress<0.0)return UMI_STATUS_INVALID_ARGUMENT; memset(r,0,sizeof *r); s=umi_quant_copy_text(r->portfolio_id,sizeof r->portfolio_id,id); if(s!=UMI_STATUS_OK)return s; r->value_at_risk=var;r->expected_shortfall=es;r->worst_stress_loss=stress;return UMI_STATUS_OK; }
