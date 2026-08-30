/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/capital_scenario.c
 *
 * PURPOSE:
 *   Represent capital scenario adjustments to earnings, losses and RWA.
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

#include "umicom/finance/prudential/capital_scenario.h"

#include <string.h>
UmiStatus umi_pru_capital_scenario_init(UmiPrudentialCapitalScenario *scenario,const char *scenario_id,double earnings_delta,double loss_delta,double rwa_multiplier) { UmiStatus s; if(scenario==NULL||scenario_id==NULL||!umi_pru_number_valid(earnings_delta)||!umi_pru_number_valid(loss_delta)||!umi_pru_number_valid(rwa_multiplier)||loss_delta<0.0||rwa_multiplier<0.0)return UMI_STATUS_INVALID_ARGUMENT; memset(scenario,0,sizeof *scenario); s=umi_pru_copy_text(scenario->scenario_id,sizeof scenario->scenario_id,scenario_id); if(s!=UMI_STATUS_OK)return s; scenario->earnings_delta=earnings_delta; scenario->loss_delta=loss_delta; scenario->rwa_multiplier=rwa_multiplier; return UMI_STATUS_OK; }
