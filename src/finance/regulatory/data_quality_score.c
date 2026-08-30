/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/data_quality_score.c
 *
 * PURPOSE:
 *   Aggregate weighted data-quality dimensions into one bounded score.
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

#include "umicom/finance/regulatory/data_quality_score.h"

UmiStatus umi_reg_data_quality_score_compute(const double *scores,const double *weights,size_t count,double *out_score){ size_t i; double sum=0.0,total=0.0; if(scores==NULL||weights==NULL||out_score==NULL||count==0U)return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<count;++i){ if(!umi_reg_number_valid(scores[i])||!umi_reg_number_valid(weights[i])||scores[i]<0.0||scores[i]>1.0||weights[i]<0.0)return UMI_STATUS_INVALID_ARGUMENT; sum+=scores[i]*weights[i];total+=weights[i]; } if(total<=0.0)return UMI_STATUS_INVALID_ARGUMENT; *out_score=sum/total; return UMI_STATUS_OK; }
