/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/bond_pricer.c
 *
 * PURPOSE:
 *   Price fixed-rate bonds from regular coupons and a flat yield.
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

#include "umicom/finance/quant/bond_pricer.h"

#include <math.h>
#include <string.h>

/*
 * Provide the quant bond pricer price operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_bond_pricer_price(double face_value,double coupon_rate,int32_t payments_per_year,int32_t periods,double yield_rate,double *out_price){int32_t i;double coupon,period_yield,pv=0.0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_price==NULL||face_value<=0.0||coupon_rate<0.0||payments_per_year<=0||periods<=0||yield_rate<=-1.0)return UMI_STATUS_INVALID_ARGUMENT;coupon=face_value*coupon_rate/(double)payments_per_year;period_yield=yield_rate/(double)payments_per_year;/* Visit each bounded item once so every record receives the same rule. */ for(i=1;i<=periods;++i)pv+=coupon/pow(1.0+period_yield,(double)i);pv+=face_value/pow(1.0+period_yield,(double)periods);*out_price=pv;return UMI_STATUS_OK;}
