/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/prudential_limit_result.c
 *
 * PURPOSE:
 *   Evaluate a prudential metric against a configured control limit.
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

#include "umicom/finance/prudential/prudential_limit_result.h"

#include <string.h>
/*
 * Provide the pru prudential limit result evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_pru_prudential_limit_result_evaluate(UmiPrudentialLimitResult *result,const UmiPrudentialLimit *limit,double observed) { UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL||limit==NULL||!umi_pru_number_valid(observed))return UMI_STATUS_INVALID_ARGUMENT; memset(result,0,sizeof *result); s=umi_pru_copy_text(result->limit_id,sizeof result->limit_id,limit->limit_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; result->observed=observed; result->threshold=limit->threshold; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(limit->is_minimum){result->headroom=observed-limit->threshold;result->breached=observed<limit->threshold?1:0;}/* Use this fallback path when the earlier condition does not apply. */ else{result->headroom=limit->threshold-observed;result->breached=observed>limit->threshold?1:0;} return UMI_STATUS_OK; }
