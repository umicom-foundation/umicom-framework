/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/decimal.c
 *
 * PURPOSE:
 *   Represent fixed-scale decimal coefficients for cash and control calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The implementation is deliberately small and deterministic so callers can test decimal behaviour without starting a complete product.
 */

#include "umicom/finance/decimal.h"
UmiStatus umi_decimal_rescale(UmiDecimal value,uint8_t target_scale,UmiDecimal *out){if(out==NULL||target_scale>9U)return UMI_STATUS_INVALID_ARGUMENT;while(value.scale<target_scale){value.coefficient*=10;value.scale++;}while(value.scale>target_scale){value.coefficient/=10;value.scale--;}*out=value;return UMI_STATUS_OK;}
