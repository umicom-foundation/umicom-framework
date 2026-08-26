/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_scenario_set.c
 *
 * PURPOSE:
 *   Maintain a bounded enterprise stress-scenario catalogue.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_scenario_set.h"

#include <string.h>
UmiStatus umi_pru_stress_scenario_set_add(UmiPrudentialStressScenarioSet *set,const UmiPrudentialStressScenario *scenario) { size_t i; if(set==NULL||scenario==NULL)return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<set->count;++i)if(strcmp(set->scenarios[i].scenario_id,scenario->scenario_id)==0)return UMI_STATUS_ALREADY_EXISTS; if(set->count>=UMI_PRU_MAX_SCENARIOS)return UMI_STATUS_CAPACITY_EXCEEDED; set->scenarios[set->count++]=*scenario; return UMI_STATUS_OK; }
