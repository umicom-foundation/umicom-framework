/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_product_descriptor.c
 *
 * PURPOSE:
 *   Exercise the product descriptor financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/product_descriptor.h"

int main(void)
{
    UmiProductDescriptor x; CHECK(umi_product_descriptor_init(&x,"ID","Name","CODE",1U)==UMI_STATUS_OK); CHECK(umi_product_descriptor_is_valid(&x));
    return 0;
}
