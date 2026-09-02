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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_suite.h"

#include <string.h>
/* Add pru stress suite only after its inputs and available capacity have been checked. */
UmiStatus umi_pru_stress_suite_add(UmiPrudentialStressSuite *suite,const char *scenario_id) { size_t i; UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(suite==NULL||scenario_id==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<suite->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(suite->scenario_ids[i],scenario_id)==0)return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(suite->count>=UMI_PRU_MAX_SCENARIOS)return UMI_STATUS_CAPACITY_EXCEEDED; s=umi_pru_copy_text(suite->scenario_ids[suite->count],UMI_PRU_ID_CAPACITY,scenario_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; ++suite->count; return UMI_STATUS_OK; }
