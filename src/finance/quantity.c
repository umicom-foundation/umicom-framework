/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quantity.c
 *
 * PURPOSE:
 *   Validate finite positive trading quantities without owning instrument semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation is deliberately small and deterministic so callers can test quantity behaviour without starting a complete product.
 */

#include "umicom/finance/quantity.h"
/* Provide the quantity positive operation used by this module and its client applications. */
int umi_quantity_positive(UmiQuantity quantity){return quantity.value>0.0 && quantity.value<1.0e15;}
/* Provide the quantity signed operation used by this module and its client applications. */
UmiQuantity umi_quantity_signed(UmiQuantity quantity,int direction){UmiQuantity result=quantity;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(direction<0)result.value=-result.value;return result;}
