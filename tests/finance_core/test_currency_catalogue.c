/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_currency_catalogue.c
 *
 * PURPOSE:
 *   Exercise the currency catalogue financial-core contract.
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
#include "umicom/finance/core/currency_catalogue.h"

int main(void)
{
    UmiCurrencyCatalogue c; UmiCurrencyDescriptor d; umi_currency_catalogue_init(&c); CHECK(umi_currency_descriptor_init(&d,"USD","US Dollar",2U)==UMI_STATUS_OK); CHECK(umi_currency_catalogue_add(&c,&d)==UMI_STATUS_OK); CHECK(umi_currency_catalogue_find(&c,"USD")!=NULL);
    return 0;
}
