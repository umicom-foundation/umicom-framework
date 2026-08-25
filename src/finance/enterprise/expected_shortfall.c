/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/expected_shortfall.c
 *
 * PURPOSE:
 *   Calculate deterministic historical expected shortfall beyond empirical VaR.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/expected_shortfall.h"

#include <stdlib.h>
static int cmp_double_es(const void *a,const void *b){ const double x=*(const double*)a,y=*(const double*)b; return (x>y)-(x<y); }
UmiStatus umi_enterprise_expected_shortfall_historical(const double *losses,size_t count,double confidence,double *out){ double *copy,sum=0.0;size_t i,index,n=0U; if(losses==NULL||out==NULL||count==0U||!umi_quant_number_valid(confidence)||confidence<=0.0||confidence>=1.0)return UMI_STATUS_INVALID_ARGUMENT; copy=(double*)malloc(count*sizeof *copy);if(copy==NULL)return UMI_STATUS_OUT_OF_MEMORY;for(i=0U;i<count;++i){if(!umi_quant_number_valid(losses[i])){free(copy);return UMI_STATUS_INVALID_ARGUMENT;}copy[i]=losses[i];}qsort(copy,count,sizeof *copy,cmp_double_es); index=(size_t)(confidence*(double)count);if(index>=count)index=count-1U;for(i=index;i<count;++i){sum+=copy[i];++n;}*out=sum/(double)n;free(copy);return UMI_STATUS_OK; }
