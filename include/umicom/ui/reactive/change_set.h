/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/change_set.h
 *
 * PURPOSE:
 *   Collect changed property paths for one atomic update.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CHANGE_SET_H
#define UMICOM_UI_REACTIVE_CHANGE_SET_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive change set data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveChangeSet {
    char paths[64][UMI_UI_REACTIVE_PATH_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiUiReactiveChangeSet;
/**
 * Initialise ui reactive change set from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_change_set_init(UmiUiReactiveChangeSet *item);
/**
 * Check that ui reactive change set satisfies its contract before another service relies
 * on it.
 */
int umi_ui_reactive_change_set_valid(const UmiUiReactiveChangeSet *item);
#ifdef __cplusplus
}
#endif
#endif
