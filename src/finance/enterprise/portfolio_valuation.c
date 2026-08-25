/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/portfolio_valuation.c
 *
 * PURPOSE:
 *   Store portfolio-level PV, previous PV and derived P&L for one valuation cut.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/portfolio_valuation.h"

#include <string.h>
UmiStatus umi_enterprise_portfolio_valuation_init(UmiEnterprisePortfolioValuation *v,const char *id,int64_t asof,double pv,double prev){ UmiStatus s; if(v==NULL||asof<0||!umi_quant_number_valid(pv)||!umi_quant_number_valid(prev))return UMI_STATUS_INVALID_ARGUMENT; memset(v,0,sizeof *v); s=umi_quant_copy_text(v->portfolio_id,sizeof v->portfolio_id,id); if(s!=UMI_STATUS_OK)return s; v->as_of_ms=asof;v->present_value=pv;v->previous_present_value=prev;v->pnl=pv-prev;return UMI_STATUS_OK; }
