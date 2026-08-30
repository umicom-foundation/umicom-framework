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
UmiStatus umi_enterprise_historical_scenario_init(UmiEnterpriseHistoricalScenario *s,const char *id){ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);return umi_quant_copy_text(s->scenario_id,sizeof s->scenario_id,id); }
UmiStatus umi_enterprise_historical_scenario_add_shock(UmiEnterpriseHistoricalScenario *s,const char *name,double shock){ size_t i;if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<s->count;++i)if(strcmp(s->shocks[i].name,name)==0)return UMI_STATUS_ALREADY_EXISTS;if(s->count>=UMI_ENTERPRISE_MAX_FACTORS)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_enterprise_metric_init(&s->shocks[s->count],name,shock)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;s->count++;return UMI_STATUS_OK; }
