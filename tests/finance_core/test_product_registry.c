/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_product_registry.c
 *
 * PURPOSE:
 *   Exercise the product registry financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr)                                                                                \
  do {                                                                                             \
    if (!(expr))                                                                                   \
      return 1;                                                                                    \
  } while (0)
#include "umicom/finance/core/product_registry.h"
#include <string.h>

int main(void) {
  UmiFinancialProductRegistry r;
  UmiProductDescriptor x;
  umi_financial_product_registry_init(&r);
  CHECK(umi_product_descriptor_init(&x, "P", "Product", "FX", 1U) == UMI_STATUS_OK);
  CHECK(umi_financial_product_registry_add(&r, &x) == UMI_STATUS_OK);
  return 0;
}
