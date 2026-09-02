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

/*
 * Provide the ws layout lock default operation used by this module and its client
 * applications.
 */
UmiWsLayoutLock umi_ws_layout_lock_default(void) {
    UmiWsLayoutLock lock = {true, true, false};
    return lock;
}

/*
 * Provide the ws layout lock allows operation used by this module and its client
 * applications.
 */
bool umi_ws_layout_lock_allows(const UmiWsLayoutLock *lock, UmiWsLayoutMutation mutation) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lock == NULL) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!lock->locked) return true;
    /* Apply this branch only when its contract condition is satisfied. */
    if (mutation == UMI_WS_LAYOUT_MUTATION_RESIZE) return lock->allow_resize_when_locked;
    /* Apply this branch only when its contract condition is satisfied. */
    if (mutation == UMI_WS_LAYOUT_MUTATION_CLOSE) return lock->allow_close_when_locked;
    return false;
}

/*
 * Copy ws layout lock into module-owned storage so callers keep ownership of their input
 * values.
 */
void umi_ws_layout_lock_set(UmiWsLayoutLock *lock, bool locked) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lock != NULL) lock->locked = locked;
}
