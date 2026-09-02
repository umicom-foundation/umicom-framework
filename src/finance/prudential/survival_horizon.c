/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/survival_horizon.c
 *
 * PURPOSE:
 *   Calculate survival horizon from daily net flows and opening liquidity.
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

#include "umicom/finance/prudential/survival_horizon.h"

/*
 * Provide the pru survival horizon calculate operation used by this module and its client
 * applications.
 */
UmiStatus umi_pru_survival_horizon_calculate(UmiPrudentialSurvivalHorizon *result,double opening,const double *flows,size_t count) { size_t i; double balance; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL||flows==NULL||count==0U||count>UMI_PRU_MAX_PERIODS||opening<0.0)return UMI_STATUS_INVALID_ARGUMENT; balance=opening; result->opening_liquidity=opening; result->survived_days=0; result->exhausted=0; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<count;++i) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_pru_number_valid(flows[i]))return UMI_STATUS_INVALID_ARGUMENT; balance+=flows[i]; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(balance<0.0){result->exhausted=1;break;} ++result->survived_days; } result->closing_liquidity=balance; return UMI_STATUS_OK; }
