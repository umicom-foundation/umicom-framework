/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/value_at_risk.c
 *
 * PURPOSE:
 *   Calculate deterministic historical value-at-risk from loss observations.
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

#include "umicom/finance/enterprise/value_at_risk.h"

#include <stdlib.h>
static int cmp_double(const void *a,const void *b){ const double x=*(const double*)a,y=*(const double*)b; return (x>y)-(x<y); }
UmiStatus umi_enterprise_value_at_risk_historical(const double *losses,size_t count,double confidence,double *out){ double *copy;size_t i,index; if(losses==NULL||out==NULL||count==0U||!umi_quant_number_valid(confidence)||confidence<=0.0||confidence>=1.0)return UMI_STATUS_INVALID_ARGUMENT; copy=(double*)malloc(count*sizeof *copy);if(copy==NULL)return UMI_STATUS_OUT_OF_MEMORY;for(i=0U;i<count;++i){if(!umi_quant_number_valid(losses[i])){free(copy);return UMI_STATUS_INVALID_ARGUMENT;}copy[i]=losses[i];}qsort(copy,count,sizeof *copy,cmp_double); index=(size_t)(confidence*(double)count); if(index>=count)index=count-1U; *out=copy[index];free(copy);return UMI_STATUS_OK; }
