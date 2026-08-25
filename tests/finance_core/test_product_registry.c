/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_product_registry.c
 *
 * PURPOSE:
 *   Exercise the product registry financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/product_registry.h"

int main(void)
{
    UmiProductRegistry r; UmiProductDescriptor x; umi_product_registry_init(&r); CHECK(umi_product_descriptor_init(&x,"P","Product","FX",1U)==UMI_STATUS_OK); CHECK(umi_product_registry_add(&r,&x)==UMI_STATUS_OK);
    return 0;
}
