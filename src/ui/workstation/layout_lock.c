/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_lock.c
 *
 * PURPOSE:
 *   Implement explicit layout lock policy so normal work cannot accidentally move or close protected surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/layout_lock.h"

UmiWsLayoutLock umi_ws_layout_lock_default(void) {
    UmiWsLayoutLock lock = {true, true, false};
    return lock;
}

bool umi_ws_layout_lock_allows(const UmiWsLayoutLock *lock, UmiWsLayoutMutation mutation) {
    if (lock == NULL) return false;
    if (!lock->locked) return true;
    if (mutation == UMI_WS_LAYOUT_MUTATION_RESIZE) return lock->allow_resize_when_locked;
    if (mutation == UMI_WS_LAYOUT_MUTATION_CLOSE) return lock->allow_close_when_locked;
    return false;
}

void umi_ws_layout_lock_set(UmiWsLayoutLock *lock, bool locked) {
    if (lock != NULL) lock->locked = locked;
}
