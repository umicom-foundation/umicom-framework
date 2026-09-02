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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_scenario_set.h"

#include <string.h>
/*
 * Add pru stress scenario set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_pru_stress_scenario_set_add(UmiPrudentialStressScenarioSet *set,const UmiPrudentialStressScenario *scenario) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(set==NULL||scenario==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<set->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(set->scenarios[i].scenario_id,scenario->scenario_id)==0)return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(set->count>=UMI_PRU_MAX_SCENARIOS)return UMI_STATUS_CAPACITY_EXCEEDED; set->scenarios[set->count++]=*scenario; return UMI_STATUS_OK; }
