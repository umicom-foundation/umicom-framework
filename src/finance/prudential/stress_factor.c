/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_factor.c
 *
 * PURPOSE:
 *   Define a named multiplicative stress factor with direction and severity.
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

#include "umicom/finance/prudential/stress_factor.h"

#include <string.h>
/*
 * Initialise pru stress factor from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_pru_stress_factor_init(UmiPrudentialStressFactor *factor,const char *factor_id,UmiStressDirection direction,double magnitude,UmiPrudentialSeverity severity) { UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(factor==NULL||factor_id==NULL||magnitude<0.0||!umi_pru_number_valid(magnitude)||(direction!=UMI_PRU_STRESS_DECREASE&&direction!=UMI_PRU_STRESS_INCREASE))return UMI_STATUS_INVALID_ARGUMENT; memset(factor,0,sizeof *factor); s=umi_pru_copy_text(factor->factor_id,sizeof factor->factor_id,factor_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; factor->direction=direction; factor->magnitude=magnitude; factor->severity=severity; return UMI_STATUS_OK; }
