/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/position_aggregation.c
 *
 * PURPOSE:
 *   Implement position quantity aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/position_aggregation.h"

#include <string.h>
/* Sum all positions. */ UmiStatus
umi_position_aggregation_total(const UmiFinancialPositionBook *book, double *out) {
  size_t i;
  double t = 0.0;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (book == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Visit each bounded item once so every record receives the same rule. */
  for (i = 0U; i < book->count; i++)
    t += book->items[i].quantity.value;
  *out = t;
  return UMI_STATUS_OK;
}
/* Sum positions for one product. */ UmiStatus
umi_position_aggregation_product(const UmiFinancialPositionBook *book, const char *product,
                                 double *out) {
  size_t i;
  double t = 0.0;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (book == NULL || product == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Visit each bounded item once so every record receives the same rule. */
  for (i = 0U; i < book->count; i++)
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (strcmp(book->items[i].key.product_id.id.value, product) == 0)
      t += book->items[i].quantity.value;
  *out = t;
  return UMI_STATUS_OK;
}
