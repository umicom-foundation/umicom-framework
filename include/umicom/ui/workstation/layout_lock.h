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

/**
 * List the named ws layout mutation values accepted by this public contract.
 */
typedef enum UmiWsLayoutMutation {
    UMI_WS_LAYOUT_MUTATION_MOVE = 1,
    UMI_WS_LAYOUT_MUTATION_RESIZE = 2,
    UMI_WS_LAYOUT_MUTATION_CLOSE = 3,
    UMI_WS_LAYOUT_MUTATION_ADD = 4
} UmiWsLayoutMutation;

/**
 * Represent the ws layout lock data shared with callers of this public contract.
 */
typedef struct UmiWsLayoutLock {
    bool locked;
    bool allow_resize_when_locked;
    bool allow_close_when_locked;
} UmiWsLayoutLock;

/**
 * Provide the ws layout lock default operation used by this module and its client
 * applications.
 */
UmiWsLayoutLock umi_ws_layout_lock_default(void);
/**
 * Provide the ws layout lock allows operation used by this module and its client
 * applications.
 */
bool umi_ws_layout_lock_allows(const UmiWsLayoutLock *lock, UmiWsLayoutMutation mutation);
/**
 * Copy ws layout lock into module-owned storage so callers keep ownership of their input
 * values.
 */
void umi_ws_layout_lock_set(UmiWsLayoutLock *lock, bool locked);

#ifdef __cplusplus
}
#endif

#endif
