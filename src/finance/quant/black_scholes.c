/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/black_scholes.c
 *
 * PURPOSE:
 *   Price European equity-style options with continuous rates and dividends.
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

#include "umicom/finance/quant/black_scholes.h"

#include <math.h>
#include <string.h>

UmiStatus umi_quant_black_scholes_price(double spot,double strike,double risk_free_rate,double dividend_yield,double volatility,double years,UmiQuantOptionRight right,double *out_price){double sigma_t,d1,d2,df_r,df_q;if(out_price==NULL||spot<=0.0||strike<=0.0||volatility<0.0||years<0.0||(right!=UMI_QUANT_CALL&&right!=UMI_QUANT_PUT))return UMI_STATUS_INVALID_ARGUMENT;if(years==0.0||volatility==0.0){*out_price=fmax((double)right*(spot-strike),0.0);return UMI_STATUS_OK;}sigma_t=volatility*sqrt(years);d1=(log(spot/strike)+(risk_free_rate-dividend_yield+0.5*volatility*volatility)*years)/sigma_t;d2=d1-sigma_t;df_r=exp(-risk_free_rate*years);df_q=exp(-dividend_yield*years);if(right==UMI_QUANT_CALL)*out_price=spot*df_q*umi_quant_normal_cdf(d1)-strike*df_r*umi_quant_normal_cdf(d2);else *out_price=strike*df_r*umi_quant_normal_cdf(-d2)-spot*df_q*umi_quant_normal_cdf(-d1);return UMI_STATUS_OK;}
