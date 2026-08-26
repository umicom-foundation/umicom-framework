/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/state_restore.h
 *
 * PURPOSE:
 *   Represent governed state restoration intent and result.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_STATE_RESTORE_H
#define UMICOM_UI_REACTIVE_STATE_RESTORE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveStateRestore {
    char snapshot_id[UMI_UI_REACTIVE_ID_CAPACITY];
    uint64_t from_revision;
    uint64_t to_revision;
    bool completed;
} UmiUiReactiveStateRestore;
void umi_ui_reactive_state_restore_init(UmiUiReactiveStateRestore *item);
int umi_ui_reactive_state_restore_valid(const UmiUiReactiveStateRestore *item);
#ifdef __cplusplus
}
#endif
#endif
