/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/cancellation.h
 *
 * PURPOSE:
 *   Provide cooperative cancellation for long-running product completion executions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_CANCELLATION_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_CANCELLATION_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the product execution cancellation data shared with callers of this public
 * contract.
 */
typedef struct UmiProductExecutionCancellation {
    uint64_t revision;
    int requested;
} UmiProductExecutionCancellation;
/** Clear a cooperative cancellation token before work begins. */
void umi_product_execution_cancellation_init(UmiProductExecutionCancellation *cancellation);
/** Mark a token so long-running operations may stop at a safe boundary. */
void umi_product_execution_cancellation_request(UmiProductExecutionCancellation *cancellation);
/** Clear a previous request before a new execution run. */
void umi_product_execution_cancellation_reset(UmiProductExecutionCancellation *cancellation);
/** Return non-zero when cancellation has been requested. */
int umi_product_execution_cancellation_requested(
    const UmiProductExecutionCancellation *cancellation);
#ifdef __cplusplus
}
#endif
#endif
