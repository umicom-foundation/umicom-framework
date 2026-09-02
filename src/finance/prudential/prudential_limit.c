/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/prudential_limit.c
 *
 * PURPOSE:
 *   Define a prudential minimum or maximum control limit.
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

#include "umicom/finance/prudential/prudential_limit.h"

#include <string.h>
/*
 * Initialise pru prudential limit from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_pru_prudential_limit_init(UmiPrudentialLimit *limit,const char *limit_id,double threshold,int is_minimum,UmiPrudentialSeverity severity) { UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(limit==NULL||limit_id==NULL||!umi_pru_number_valid(threshold)||(is_minimum!=0&&is_minimum!=1))return UMI_STATUS_INVALID_ARGUMENT; memset(limit,0,sizeof *limit); s=umi_pru_copy_text(limit->limit_id,sizeof limit->limit_id,limit_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; limit->threshold=threshold; limit->is_minimum=is_minimum; limit->severity=severity; return UMI_STATUS_OK; }
