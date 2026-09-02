/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/decimal.c
 *
 * PURPOSE:
 *   Represent fixed-scale decimal coefficients for cash and control calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation is deliberately small and deterministic so callers can test decimal behaviour without starting a complete product.
 */

#include "umicom/finance/decimal.h"
/* Provide the decimal rescale operation used by this module and its client applications. */
UmiStatus umi_decimal_rescale(UmiDecimal value,uint8_t target_scale,UmiDecimal *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL||target_scale>9U)return UMI_STATUS_INVALID_ARGUMENT;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(value.scale<target_scale){value.coefficient*=10;value.scale++;}/* Continue only while work remains available; the loop body advances the state on each pass. */ while(value.scale>target_scale){value.coefficient/=10;value.scale--;}*out=value;return UMI_STATUS_OK;}
