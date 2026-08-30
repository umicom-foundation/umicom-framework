/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/credit_curve.c
 *
 * PURPOSE:
 *   Store bounded hazard-rate term structures.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/credit_curve.h"

#include <math.h>
#include <string.h>

void umi_quant_credit_curve_init(UmiQuantCreditCurve *curve){if(curve!=NULL)memset(curve,0,sizeof *curve);}
UmiStatus umi_quant_credit_curve_add(UmiQuantCreditCurve *curve,int32_t tenor_days,double hazard_rate){if(curve==NULL||tenor_days<0||hazard_rate<0.0)return UMI_STATUS_INVALID_ARGUMENT;if(curve->count>=UMI_QUANT_MAX_CURVE_NODES)return UMI_STATUS_CAPACITY_EXCEEDED;if(curve->count>0U&&tenor_days<=curve->hazards[curve->count-1U].tenor_days)return UMI_STATUS_INVALID_STATE;curve->hazards[curve->count++]=(UmiQuantCurvePoint){tenor_days,hazard_rate};return UMI_STATUS_OK;}
UmiStatus umi_quant_credit_curve_hazard(const UmiQuantCreditCurve *curve,int32_t tenor_days,double *out_hazard){size_t i;if(curve==NULL||out_hazard==NULL||curve->count==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<curve->count;++i)if(tenor_days<=curve->hazards[i].tenor_days){*out_hazard=curve->hazards[i].value;return UMI_STATUS_OK;}*out_hazard=curve->hazards[curve->count-1U].value;return UMI_STATUS_OK;}
