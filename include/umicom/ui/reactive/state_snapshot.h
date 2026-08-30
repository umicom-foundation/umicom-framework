/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/state_snapshot.h
 *
 * PURPOSE:
 *   Represent a named immutable state snapshot reference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_STATE_SNAPSHOT_H
#define UMICOM_UI_REACTIVE_STATE_SNAPSHOT_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveStateSnapshot {
    char snapshot_id[UMI_UI_REACTIVE_ID_CAPACITY];
    uint64_t revision;
    size_t property_count;
    uint64_t fingerprint;
} UmiUiReactiveStateSnapshot;
void umi_ui_reactive_state_snapshot_init(UmiUiReactiveStateSnapshot *item);
int umi_ui_reactive_state_snapshot_valid(const UmiUiReactiveStateSnapshot *item);
#ifdef __cplusplus
}
#endif
#endif
