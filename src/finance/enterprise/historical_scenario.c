/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/historical_scenario.c
 *
 * PURPOSE:
 *   Represent named historical factor shocks for portfolio replay.
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

#include "umicom/finance/enterprise/historical_scenario.h"

#include <string.h>
/*
 * Initialise enterprise historical scenario from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_enterprise_historical_scenario_init(UmiEnterpriseHistoricalScenario *s,const char *id){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);return umi_quant_copy_text(s->scenario_id,sizeof s->scenario_id,id); }
/*
 * Provide the enterprise historical scenario add shock operation used by this module and
 * its client applications.
 */
UmiStatus umi_enterprise_historical_scenario_add_shock(UmiEnterpriseHistoricalScenario *s,const char *name,double shock){ size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->shocks[i].name,name)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_ENTERPRISE_MAX_FACTORS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_enterprise_metric_init(&s->shocks[s->count],name,shock)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;s->count++;return UMI_STATUS_OK; }
