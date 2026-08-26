/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/batch_update.h
 *
 * PURPOSE:
 *   Aggregate state mutations into one revision boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BATCH_UPDATE_H
#define UMICOM_UI_REACTIVE_BATCH_UPDATE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveBatchUpdate {
    size_t mutation_count;
    uint64_t start_revision;
    uint64_t end_revision;
    bool committed;
} UmiUiReactiveBatchUpdate;
void umi_ui_reactive_batch_update_init(UmiUiReactiveBatchUpdate *item);
int umi_ui_reactive_batch_update_valid(const UmiUiReactiveBatchUpdate *item);
#ifdef __cplusplus
}
#endif
#endif
