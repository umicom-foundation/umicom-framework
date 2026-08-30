/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/layout_normalizer.c
 *
 * PURPOSE:
 *   ratio, order and geometry normalisation before cross-frontend layout comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/layout_normalizer.h"

void umi_fc_layout_normalizer_ratios(double *values,size_t count,double minimum,double maximum){size_t i;if(values==NULL)return;if(minimum>maximum){double t=minimum;minimum=maximum;maximum=t;}for(i=0U;i<count;++i){if(values[i]<minimum)values[i]=minimum;if(values[i]>maximum)values[i]=maximum;}}
void umi_fc_layout_normalizer_orders(int32_t *orders,size_t count){size_t i,j;if(orders==NULL)return;for(i=0U;i<count;++i)for(j=i+1U;j<count;++j)if(orders[j]<orders[i]){int32_t t=orders[i];orders[i]=orders[j];orders[j]=t;}}
