/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/black76.c
 *
 * PURPOSE:
 *   Price European options on forwards using the Black 1976 model.
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

#include "umicom/finance/quant/black76.h"

#include <math.h>
#include <string.h>

/*
 * Provide the quant black76 price operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_black76_price(double forward,double strike,double volatility,double years,double discount_factor,UmiQuantOptionRight right,double *out_price){double sigma_t,d1,d2;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_price==NULL||forward<=0.0||strike<=0.0||volatility<0.0||years<0.0||discount_factor<0.0||(right!=UMI_QUANT_CALL&&right!=UMI_QUANT_PUT))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(years==0.0||volatility==0.0){*out_price=discount_factor*fmax((double)right*(forward-strike),0.0);return UMI_STATUS_OK;}sigma_t=volatility*sqrt(years);d1=(log(forward/strike)+0.5*volatility*volatility*years)/sigma_t;d2=d1-sigma_t;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(right==UMI_QUANT_CALL)*out_price=discount_factor*(forward*umi_quant_normal_cdf(d1)-strike*umi_quant_normal_cdf(d2));/* Use this fallback path when the earlier condition does not apply. */ else *out_price=discount_factor*(strike*umi_quant_normal_cdf(-d2)-forward*umi_quant_normal_cdf(-d1));return UMI_STATUS_OK;}
