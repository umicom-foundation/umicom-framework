/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_scenario.c
 *
 * PURPOSE:
 *   Define one governed enterprise stress scenario and its severity.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_scenario.h"

#include <string.h>
UmiStatus umi_pru_stress_scenario_init(UmiPrudentialStressScenario *scenario,const char *scenario_id,const char *display_name,UmiPrudentialSeverity severity,uint32_t horizon_days) { UmiStatus s; if(scenario==NULL||scenario_id==NULL||display_name==NULL||horizon_days==0U)return UMI_STATUS_INVALID_ARGUMENT; memset(scenario,0,sizeof *scenario); s=umi_pru_copy_text(scenario->scenario_id,sizeof scenario->scenario_id,scenario_id); if(s!=UMI_STATUS_OK)return s; s=umi_pru_copy_text(scenario->name,sizeof scenario->name,display_name); if(s!=UMI_STATUS_OK)return s; scenario->severity=severity; scenario->horizon_days=horizon_days; return UMI_STATUS_OK; }
