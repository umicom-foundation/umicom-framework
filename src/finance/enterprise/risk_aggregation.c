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
/*
 * Provide the enterprise risk aggregate rss operation used by this module and its client
 * applications.
 */
UmiStatus umi_enterprise_risk_aggregate_rss(const double *risks,const double *weights,size_t count,double *out){ size_t i;double sum=0.0,term;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(risks==NULL||weights==NULL||out==NULL||count==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_quant_number_valid(risks[i])||!umi_quant_number_valid(weights[i]))return UMI_STATUS_INVALID_ARGUMENT;term=risks[i]*weights[i];sum+=term*term;}*out=sqrt(sum);return UMI_STATUS_OK; }
