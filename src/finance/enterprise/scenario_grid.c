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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/scenario_grid.h"

#include <string.h>
/*
 * Initialise enterprise scenario grid from caller-provided values so later operations
 * receive a known state.
 */
void umi_enterprise_scenario_grid_init(UmiEnterpriseScenarioGrid *g){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g!=NULL)memset(g,0,sizeof *g);}
/*
 * Add enterprise scenario grid only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_enterprise_scenario_grid_add(UmiEnterpriseScenarioGrid *g,const UmiEnterpriseHistoricalScenario *s){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(g->scenarios[i].scenario_id,s->scenario_id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->count>=UMI_ENTERPRISE_MAX_SCENARIOS)return UMI_STATUS_CAPACITY_EXCEEDED;g->scenarios[g->count++]=*s;return UMI_STATUS_OK;}
