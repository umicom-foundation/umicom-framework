/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/breakpoint_edit_private.h
 * PURPOSE: Keep breakpoint mutation inside the authoritative debugger workspace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_BREAKPOINT_EDIT_PRIVATE_H
#define UMICOM_DEBUG_BREAKPOINT_EDIT_PRIVATE_H
#include "umicom/debug/breakpoint_edit.h"
struct UmiDebugBreakpointEdit {
    UmiDebugViewStamp stamp;
    UmiDebugBreakpointSnapshot before;
};
UmiStatus UmiDebugWorkspaceCommitBreakpoint(UmiDebugWorkspace *workspace,
    const UmiDebugBreakpointSnapshot *before, const UmiDebugBreakpointSnapshot *replacement);
#endif
