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

/*
 * Provide the fc layout normalizer ratios operation used by this module and its client
 * applications.
 */
void umi_fc_layout_normalizer_ratios(double *values,size_t count,double minimum,double maximum){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values==NULL)return;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(minimum>maximum){double t=minimum;minimum=maximum;maximum=t;}/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[i]<minimum)values[i]=minimum;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[i]>maximum)values[i]=maximum;}}
/*
 * Provide the fc layout normalizer orders operation used by this module and its client
 * applications.
 */
void umi_fc_layout_normalizer_orders(int32_t *orders,size_t count){size_t i,j;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(orders==NULL)return;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<count;++i)/* Visit each bounded item once so every record receives the same rule. */ for(j=i+1U;j<count;++j)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(orders[j]<orders[i]){int32_t t=orders[i];orders[i]=orders[j];orders[j]=t;}}
