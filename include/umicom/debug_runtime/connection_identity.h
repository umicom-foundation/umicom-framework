/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/connection_identity.h
 * PURPOSE: Capture debugger connection identity independently of the current inspection view.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_CONNECTION_IDENTITY_H
#define UMICOM_DEBUG_RUNTIME_CONNECTION_IDENTITY_H
#include "umicom/debug_runtime/platform.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiDebugConnectionIdentity
    {
        char session_id[128];
        uint64_t generation;
    } UmiDebugConnectionIdentity;
    /** Copy the currently active native debugger connection without performing I/O.
 * Each initialization and restart attempt receives a new generation, even when a host reuses its
 * session name. Keep this value with delayed inspection actions. A copy is
 * process-local evidence, not a credential or a persisted debugger handle.
 * Failure leaves out unchanged; disconnected platforms return NOT_FOUND.
 * Pending lifecycle events or an uncertain restart return BUSY until resolved. */
    UmiStatus UmiDebugRuntimeConnectionIdentityRead(const UmiDebugRuntimePlatform *platform,
                                                    UmiDebugConnectionIdentity *out);
#ifdef __cplusplus
}
#endif
#endif
