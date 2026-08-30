/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/snapshot.h
 *
 * PURPOSE:
 *   Expose immutable operational snapshots for Studio and other thin clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_SNAPSHOT_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_SNAPSHOT_H
#include "umicom/application/productisation/execution/acceptance.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductExecutionSnapshot {
    uint64_t revision;
    UmiProductExecutionMetrics metrics;
    UmiProductExecutionAcceptance acceptance;
    size_t current_item;
    int has_current_item;
    int cancellation_requested;
} UmiProductExecutionSnapshot;
#ifdef __cplusplus
}
#endif
#endif
