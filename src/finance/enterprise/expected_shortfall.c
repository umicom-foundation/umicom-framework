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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/expected_shortfall.h"

#include <stdlib.h>
/* Provide the cmp double es operation used by this module and its client applications. */
static int cmp_double_es(const void *a,const void *b){ const double x=*(const double*)a,y=*(const double*)b; return (x>y)-(x<y); }
/*
 * Provide the enterprise expected shortfall historical operation used by this module and
 * its client applications.
 */
UmiStatus umi_enterprise_expected_shortfall_historical(const double *losses,size_t count,double confidence,double *out){ double *copy,sum=0.0;size_t i,index,n=0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(losses==NULL||out==NULL||count==0U||!umi_quant_number_valid(confidence)||confidence<=0.0||confidence>=1.0)return UMI_STATUS_INVALID_ARGUMENT; copy=(double*)malloc(count*sizeof *copy);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(copy==NULL)return UMI_STATUS_OUT_OF_MEMORY;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_quant_number_valid(losses[i])){free(copy);return UMI_STATUS_INVALID_ARGUMENT;}copy[i]=losses[i];}qsort(copy,count,sizeof *copy,cmp_double_es); index=(size_t)(confidence*(double)count);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(index>=count)index=count-1U;/* Visit each bounded item once so every record receives the same rule. */ for(i=index;i<count;++i){sum+=copy[i];++n;}*out=sum/(double)n;free(copy);return UMI_STATUS_OK; }
