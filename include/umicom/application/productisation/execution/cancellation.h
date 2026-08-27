/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/cancellation.h
 *
 * PURPOSE:
 *   Provide cooperative cancellation for long-running product completion executions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_CANCELLATION_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_CANCELLATION_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductExecutionCancellation {
    uint64_t revision;
    int requested;
} UmiProductExecutionCancellation;
void umi_product_execution_cancellation_init(UmiProductExecutionCancellation *cancellation);
void umi_product_execution_cancellation_request(UmiProductExecutionCancellation *cancellation);
void umi_product_execution_cancellation_reset(UmiProductExecutionCancellation *cancellation);
int umi_product_execution_cancellation_requested(
    const UmiProductExecutionCancellation *cancellation);
#ifdef __cplusplus
}
#endif
#endif
