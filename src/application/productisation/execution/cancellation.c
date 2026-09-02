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
/*
 * Initialise product execution cancellation from caller-provided values so later
 * operations receive a known state.
 */
void umi_product_execution_cancellation_init(UmiProductExecutionCancellation *cancellation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cancellation != NULL) (void)memset(cancellation, 0, sizeof(*cancellation));
}
/*
 * Provide the product execution cancellation request operation used by this module and its
 * client applications.
 */
void umi_product_execution_cancellation_request(UmiProductExecutionCancellation *cancellation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cancellation != NULL) {
        cancellation->requested = 1;
        cancellation->revision++;
    }
}
/*
 * Release or reset state held by product execution cancellation so the same storage can be
 * reused safely.
 */
void umi_product_execution_cancellation_reset(UmiProductExecutionCancellation *cancellation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cancellation != NULL) {
        cancellation->requested = 0;
        cancellation->revision++;
    }
}
/*
 * Provide the product execution cancellation requested operation used by this module and
 * its client applications.
 */
int umi_product_execution_cancellation_requested(
    const UmiProductExecutionCancellation *cancellation)
{
    return cancellation != NULL && cancellation->requested;
}
