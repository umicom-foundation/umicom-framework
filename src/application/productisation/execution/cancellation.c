/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/cancellation.c
 *
 * PURPOSE:
 *   Implement revisioned cooperative execution cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/cancellation.h"
#include <string.h>
void umi_product_execution_cancellation_init(UmiProductExecutionCancellation *cancellation)
{
    if (cancellation != NULL) (void)memset(cancellation, 0, sizeof(*cancellation));
}
void umi_product_execution_cancellation_request(UmiProductExecutionCancellation *cancellation)
{
    if (cancellation != NULL) {
        cancellation->requested = 1;
        cancellation->revision++;
    }
}
void umi_product_execution_cancellation_reset(UmiProductExecutionCancellation *cancellation)
{
    if (cancellation != NULL) {
        cancellation->requested = 0;
        cancellation->revision++;
    }
}
int umi_product_execution_cancellation_requested(
    const UmiProductExecutionCancellation *cancellation)
{
    return cancellation != NULL && cancellation->requested;
}
