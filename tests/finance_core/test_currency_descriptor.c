/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_currency_descriptor.c
 *
 * PURPOSE:
 *   Exercise the currency descriptor financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/currency_descriptor.h"

int main(void)
{
    UmiCurrencyDescriptor d; CHECK(umi_currency_descriptor_init(&d,"gbp","Pound Sterling",2U)==UMI_STATUS_OK); CHECK(umi_currency_descriptor_matches(&d,"GBP"));
    return 0;
}
