/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/state_restore.h
 *
 * PURPOSE:
 *   Represent governed state restoration intent and result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_STATE_RESTORE_H
#define UMICOM_UI_REACTIVE_STATE_RESTORE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive state restore data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveStateRestore {
    char snapshot_id[UMI_UI_REACTIVE_ID_CAPACITY];
    uint64_t from_revision;
    uint64_t to_revision;
    bool completed;
} UmiUiReactiveStateRestore;
/**
 * Initialise ui reactive state restore from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_state_restore_init(UmiUiReactiveStateRestore *item);
/**
 * Check that ui reactive state restore satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_state_restore_valid(const UmiUiReactiveStateRestore *item);
#ifdef __cplusplus
}
#endif
#endif
