/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/scenario_set.c
 *
 * PURPOSE:
 *   Store bounded deterministic market scenarios.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/scenario_set.h"

#include <math.h>
#include <string.h>

/*
 * Initialise quant scenario set from caller-provided values so later operations receive a
 * known state.
 */
void umi_quant_scenario_set_init(UmiQuantScenarioSet *set){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(set!=NULL)memset(set,0,sizeof *set);}
/* Add quant scenario set only after its inputs and available capacity have been checked. */
UmiStatus umi_quant_scenario_set_add(UmiQuantScenarioSet *set,double rate_shift,double spot_shift,double vol_shift,double weight){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(set==NULL||weight<0.0)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(set->count>=UMI_QUANT_MAX_SCENARIOS)return UMI_STATUS_CAPACITY_EXCEEDED;set->scenarios[set->count++]=(UmiQuantScenarioPoint){rate_shift,spot_shift,vol_shift,weight};return UMI_STATUS_OK;}
/*
 * Provide the quant scenario set total weight operation used by this module and its client
 * applications.
 */
double umi_quant_scenario_set_total_weight(const UmiQuantScenarioSet *set){size_t i;double total=0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(set==NULL)return 0.0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<set->count;++i)total+=set->scenarios[i].weight;return total;}
