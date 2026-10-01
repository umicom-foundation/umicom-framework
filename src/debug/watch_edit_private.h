/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/watch_edit_private.h
 * PURPOSE: Keep guarded watch mutation with the authoritative debugger workspace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_WATCH_EDIT_PRIVATE_H
#define UMICOM_DEBUG_WATCH_EDIT_PRIVATE_H
#include "umicom/debug/watch_edit.h"
struct UmiDebugWatchEdit { UmiDebugViewStamp stamp; UmiDebugWatchSnapshot before; };
UmiDebugService *UmiDebugWorkspaceWatchService(UmiDebugWorkspace *workspace);
UmiStatus UmiDebugWorkspaceCommitWatch(UmiDebugWorkspace *workspace,
    const UmiDebugWatchSnapshot *before, const UmiDebugWatchSnapshot *replacement);
#endif
