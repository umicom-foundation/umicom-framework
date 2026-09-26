/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/session_unsupported.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "session_internal.h"
UmiStatus UmiDesktopSessionInspect(const UmiDesktopSessionRequest *request, UmiDesktopSessionCheck *outCheck)
{
    (void)request;
    return UmiDesktopSessionExplain(outCheck, UMI_STATUS_NOT_IMPLEMENTED, "The desktop-session handoff is a Linux capability; ordinary Desk remains available on this host.");
}
UmiStatus UmiDesktopSessionEnter(const UmiDesktopSessionRequest *request, UmiDesktopSessionCheck *outCheck)
{ return UmiDesktopSessionInspect(request, outCheck); }
