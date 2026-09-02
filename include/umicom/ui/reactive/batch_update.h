/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/batch_update.h
 *
 * PURPOSE:
 *   Aggregate state mutations into one revision boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BATCH_UPDATE_H
#define UMICOM_UI_REACTIVE_BATCH_UPDATE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive batch update data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveBatchUpdate {
    size_t mutation_count;
    uint64_t start_revision;
    uint64_t end_revision;
    bool committed;
} UmiUiReactiveBatchUpdate;
/**
 * Initialise ui reactive batch update from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_batch_update_init(UmiUiReactiveBatchUpdate *item);
/**
 * Check that ui reactive batch update satisfies its contract before another service relies
 * on it.
 */
int umi_ui_reactive_batch_update_valid(const UmiUiReactiveBatchUpdate *item);
#ifdef __cplusplus
}
#endif
#endif
