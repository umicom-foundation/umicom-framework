/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_position_book.c
 *
 * PURPOSE:
 *   Exercise the position book financial-core contract.
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
#include "umicom/finance/core/position_book.h"
#include <string.h>

int main(void) {
  UmiFinancialPositionBook b;
  UmiPositionKey k;
  UmiPosition p;
  umi_financial_position_book_init(&b);
  CHECK(umi_position_key_init(&k, "B", "P", "X") == UMI_STATUS_OK);
  CHECK(umi_position_init(&p, &k, (UmiQuantity){1.0}, (UmiFinancialDate){2026, 8U, 25U}) ==
        UMI_STATUS_OK);
  CHECK(umi_financial_position_book_add(&b, &p) == UMI_STATUS_OK);
  return 0;
}
