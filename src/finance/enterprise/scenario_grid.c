/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/scenario_grid.c
 *
 * PURPOSE:
 *   Maintain a bounded catalogue of unique historical scenarios for distributed risk runs.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/scenario_grid.h"

#include <string.h>
void umi_enterprise_scenario_grid_init(UmiEnterpriseScenarioGrid *g){if(g!=NULL)memset(g,0,sizeof *g);}
UmiStatus umi_enterprise_scenario_grid_add(UmiEnterpriseScenarioGrid *g,const UmiEnterpriseHistoricalScenario *s){size_t i;if(g==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<g->count;++i)if(strcmp(g->scenarios[i].scenario_id,s->scenario_id)==0)return UMI_STATUS_ALREADY_EXISTS;if(g->count>=UMI_ENTERPRISE_MAX_SCENARIOS)return UMI_STATUS_CAPACITY_EXCEEDED;g->scenarios[g->count++]=*s;return UMI_STATUS_OK;}
