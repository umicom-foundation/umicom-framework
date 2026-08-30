/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/binomial_tree.c
 *
 * PURPOSE:
 *   Price bounded-step European options with a Cox-Ross-Rubinstein tree.
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

#include "umicom/finance/quant/binomial_tree.h"

#include <math.h>
#include <string.h>

UmiStatus umi_quant_binomial_tree_price(double spot,double strike,double rate,double volatility,double years,uint32_t steps,UmiQuantOptionRight right,double *out_price){double values[257];double dt,u,d,growth,p,disc;uint32_t i,j;if(out_price==NULL||spot<=0.0||strike<=0.0||volatility<=0.0||years<=0.0||steps==0U||steps>256U)return UMI_STATUS_INVALID_ARGUMENT;dt=years/(double)steps;u=exp(volatility*sqrt(dt));d=1.0/u;growth=exp(rate*dt);p=(growth-d)/(u-d);if(p<0.0||p>1.0)return UMI_STATUS_INVALID_STATE;disc=exp(-rate*dt);for(j=0U;j<=steps;++j){double terminal=spot*pow(u,(double)j)*pow(d,(double)(steps-j));values[j]=fmax((double)right*(terminal-strike),0.0);}for(i=steps;i>0U;--i)for(j=0U;j<i;++j)values[j]=disc*(p*values[j+1U]+(1.0-p)*values[j]);*out_price=values[0];return UMI_STATUS_OK;}
