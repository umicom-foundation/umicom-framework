/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_suite.c
 *
 * PURPOSE:
 *   Aggregate scenario identifiers into one governed enterprise stress suite.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_suite.h"

#include <string.h>
UmiStatus umi_pru_stress_suite_add(UmiPrudentialStressSuite *suite,const char *scenario_id) { size_t i; UmiStatus s; if(suite==NULL||scenario_id==NULL)return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<suite->count;++i)if(strcmp(suite->scenario_ids[i],scenario_id)==0)return UMI_STATUS_ALREADY_EXISTS; if(suite->count>=UMI_PRU_MAX_SCENARIOS)return UMI_STATUS_CAPACITY_EXCEEDED; s=umi_pru_copy_text(suite->scenario_ids[suite->count],UMI_PRU_ID_CAPACITY,scenario_id); if(s!=UMI_STATUS_OK)return s; ++suite->count; return UMI_STATUS_OK; }
