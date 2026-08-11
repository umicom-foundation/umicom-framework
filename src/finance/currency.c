/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/currency.c
 *
 * PURPOSE:
 *   Validate three-letter uppercase currency codes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The implementation is deliberately small and deterministic so callers can test currency behaviour without starting a complete product.
 */

#include "umicom/finance/currency.h"
static int upper_ascii(char c){return c>='A' && c<='Z';}
int umi_currency_valid(const UmiCurrency *currency){return currency!=NULL && upper_ascii(currency->code[0]) && upper_ascii(currency->code[1]) && upper_ascii(currency->code[2]) && currency->code[3]=='\0';}
