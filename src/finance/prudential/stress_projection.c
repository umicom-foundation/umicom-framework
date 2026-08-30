/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_projection.c
 *
 * PURPOSE:
 *   Project a base value across a bounded stress path.
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

#include "umicom/finance/prudential/stress_projection.h"

UmiStatus umi_pru_stress_projection_calculate(UmiPrudentialStressProjection *result,double base_value,const double *multipliers,size_t count) { size_t i; if(result==NULL||multipliers==NULL||count==0U||count>UMI_PRU_MAX_PERIODS||!umi_pru_number_valid(base_value))return UMI_STATUS_INVALID_ARGUMENT; result->count=count; for(i=0U;i<count;++i){if(!umi_pru_number_valid(multipliers[i])||multipliers[i]<0.0)return UMI_STATUS_INVALID_ARGUMENT; result->values[i]=base_value*multipliers[i];} return UMI_STATUS_OK; }
