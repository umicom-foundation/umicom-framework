/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/normal_model.c
 *
 * PURPOSE:
 *   Price European options with the Bachelier normal model.
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

#include "umicom/finance/quant/normal_model.h"

#include <math.h>
#include <string.h>

UmiStatus umi_quant_normal_model_price(double forward,double strike,double normal_volatility,double years,double discount_factor,UmiQuantOptionRight right,double *out_price){double stddev,d,n_pdf;if(out_price==NULL||normal_volatility<0.0||years<0.0||discount_factor<0.0||(right!=UMI_QUANT_CALL&&right!=UMI_QUANT_PUT))return UMI_STATUS_INVALID_ARGUMENT;if(years==0.0||normal_volatility==0.0){*out_price=discount_factor*fmax((double)right*(forward-strike),0.0);return UMI_STATUS_OK;}stddev=normal_volatility*sqrt(years);d=(forward-strike)/stddev;n_pdf=exp(-0.5*d*d)/sqrt(2.0*3.14159265358979323846);if(right==UMI_QUANT_CALL)*out_price=discount_factor*((forward-strike)*umi_quant_normal_cdf(d)+stddev*n_pdf);else *out_price=discount_factor*((strike-forward)*umi_quant_normal_cdf(-d)+stddev*n_pdf);return UMI_STATUS_OK;}
