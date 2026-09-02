/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/currency.c
 *
 * PURPOSE:
 *   Validate three-letter uppercase currency codes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation is deliberately small and deterministic so callers can test currency behaviour without starting a complete product.
 */

#include "umicom/finance/currency.h"
/* Provide the upper ascii operation used by this module and its client applications. */
static int upper_ascii(char c){return c>='A' && c<='Z';}
/* Check that currency satisfies its contract before another service relies on it. */
int umi_currency_valid(const UmiCurrency *currency){return currency!=NULL && upper_ascii(currency->code[0]) && upper_ascii(currency->code[1]) && upper_ascii(currency->code[2]) && currency->code[3]=='\0';}
