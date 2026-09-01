/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/business_time.c
 *
 * PURPOSE:
 *   Compare business timestamps without binding callers to an operating-system clock API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation is deliberately small and deterministic so callers can test business time behaviour without starting a complete product.
 */

#include "umicom/finance/business_time.h"
int umi_business_time_compare(UmiBusinessTime left,UmiBusinessTime right){return left.epoch_millis<right.epoch_millis?-1:(left.epoch_millis>right.epoch_millis?1:0);}
int64_t umi_business_time_elapsed_ms(UmiBusinessTime start,UmiBusinessTime end){return end.epoch_millis-start.epoch_millis;}
