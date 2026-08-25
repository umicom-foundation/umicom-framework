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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/scenario_set.h"

#include <math.h>
#include <string.h>

void umi_quant_scenario_set_init(UmiQuantScenarioSet *set){if(set!=NULL)memset(set,0,sizeof *set);}
UmiStatus umi_quant_scenario_set_add(UmiQuantScenarioSet *set,double rate_shift,double spot_shift,double vol_shift,double weight){if(set==NULL||weight<0.0)return UMI_STATUS_INVALID_ARGUMENT;if(set->count>=UMI_QUANT_MAX_SCENARIOS)return UMI_STATUS_CAPACITY_EXCEEDED;set->scenarios[set->count++]=(UmiQuantScenarioPoint){rate_shift,spot_shift,vol_shift,weight};return UMI_STATUS_OK;}
double umi_quant_scenario_set_total_weight(const UmiQuantScenarioSet *set){size_t i;double total=0.0;if(set==NULL)return 0.0;for(i=0U;i<set->count;++i)total+=set->scenarios[i].weight;return total;}
