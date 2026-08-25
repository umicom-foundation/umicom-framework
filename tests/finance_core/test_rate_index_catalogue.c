/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_rate_index_catalogue.c
 *
 * PURPOSE:
 *   Exercise the rate index catalogue financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/rate_index_catalogue.h"

int main(void)
{
    UmiRateIndexCatalogue r; UmiRateIndex x; umi_rate_index_catalogue_init(&r); CHECK(umi_rate_index_init(&x,"R","Rate","USD",1U)==UMI_STATUS_OK); CHECK(umi_rate_index_catalogue_add(&r,&x)==UMI_STATUS_OK);
    return 0;
}
