/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/risk_aggregation.c
 *
 * PURPOSE:
 *   Aggregate independent weighted risk components by root-sum-of-squares.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/risk_aggregation.h"

#include <math.h>
UmiStatus umi_enterprise_risk_aggregate_rss(const double *risks,const double *weights,size_t count,double *out){ size_t i;double sum=0.0,term;if(risks==NULL||weights==NULL||out==NULL||count==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<count;++i){if(!umi_quant_number_valid(risks[i])||!umi_quant_number_valid(weights[i]))return UMI_STATUS_INVALID_ARGUMENT;term=risks[i]*weights[i];sum+=term*term;}*out=sqrt(sum);return UMI_STATUS_OK; }
