/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_position_aggregation.c
 *
 * PURPOSE:
 *   Exercise the position aggregation financial-core contract.
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
#include "umicom/finance/core/position_aggregation.h"
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiFinancialPositionBook b;
  double t = 0.0;
  umi_financial_position_book_init(&b);
  CHECK(umi_position_aggregation_total(&b, &t) == UMI_STATUS_OK && t == 0.0);
  return 0;
}
