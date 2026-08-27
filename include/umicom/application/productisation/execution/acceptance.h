/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/acceptance.h
 *
 * PURPOSE:
 *   Evaluate completion acceptance independently from execution success.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_ACCEPTANCE_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_ACCEPTANCE_H
#include "umicom/application/productisation/execution/metrics.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductExecutionAcceptance {
    int all_items_terminal;
    int no_failures;
    int no_blockers;
    int all_delivery_succeeded;
    int accepted;
} UmiProductExecutionAcceptance;
UmiProductExecutionAcceptance umi_product_execution_acceptance_evaluate(
    const UmiProductExecutionWorkQueue *queue);
#ifdef __cplusplus
}
#endif
#endif
