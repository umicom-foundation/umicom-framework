/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/layout_lock.h
 *
 * PURPOSE:
 *   Define explicit layout lock policy so normal work cannot accidentally move or close protected surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_LAYOUT_LOCK_H
#define UMICOM_UI_WORKSTATION_LAYOUT_LOCK_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWsLayoutMutation {
    UMI_WS_LAYOUT_MUTATION_MOVE = 1,
    UMI_WS_LAYOUT_MUTATION_RESIZE = 2,
    UMI_WS_LAYOUT_MUTATION_CLOSE = 3,
    UMI_WS_LAYOUT_MUTATION_ADD = 4
} UmiWsLayoutMutation;

typedef struct UmiWsLayoutLock {
    bool locked;
    bool allow_resize_when_locked;
    bool allow_close_when_locked;
} UmiWsLayoutLock;

UmiWsLayoutLock umi_ws_layout_lock_default(void);
bool umi_ws_layout_lock_allows(const UmiWsLayoutLock *lock, UmiWsLayoutMutation mutation);
void umi_ws_layout_lock_set(UmiWsLayoutLock *lock, bool locked);

#ifdef __cplusplus
}
#endif

#endif
