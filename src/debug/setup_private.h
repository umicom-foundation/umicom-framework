/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/setup_private.h
 * PURPOSE: Keep staged debugger setup publication inside canonical owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_SETUP_PRIVATE_H
#define UMICOM_DEBUG_SETUP_PRIVATE_H
#include "umicom/debug/setup.h"
#include "umicom/debug/selection.h"
struct UmiDebugSetup
{
    UmiDebugSetupSummary summary;
    UmiDebugSetupBreakpoint breakpoints[UMI_DEBUG_SETUP_CAPACITY];
    UmiDebugSetupWatch watches[UMI_DEBUG_SETUP_CAPACITY];
};
struct UmiDebugSetupReview
{
    UmiDebugSetup *before, *after;
    UmiDebugViewStamp stamp;
};
UmiStatus UmiDebugWorkspaceCommitSetup(UmiDebugWorkspace *workspace, const UmiDebugSetup *setup);
UmiStatus UmiDebugServiceCommitSetup(UmiDebugService *service, const UmiDebugSetup *setup);
/* Called only after all allocation, validation and revision checks. These
 * internal publications preserve borrowed registry addresses and never fail. */
void UmiDebugBreakpointPublishSetup(UmiDebugBreakpointRegistry *owner,
                                    const UmiDebugBreakpointRegistry *prepared);
void UmiDebugWatchPublishSetup(UmiDebugWatchRegistry *owner, const UmiDebugWatchRegistry *prepared);
#endif
