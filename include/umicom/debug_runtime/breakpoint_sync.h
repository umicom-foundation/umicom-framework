/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/breakpoint_sync.h
 * PURPOSE: Validate complete source breakpoint requests and atomically publish adapter replies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_BREAKPOINT_SYNC_H
#define UMICOM_DEBUG_RUNTIME_BREAKPOINT_SYNC_H
#include "umicom/debug/breakpoint.h"
#include "umicom/debug_runtime/results.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Validate bounded source records and signed DAP locations. If capabilities
 * is supplied, enabled conditions/logpoints require advertised support.
 * Disabled records need no optional capability. No request is sent. */
UmiStatus UmiDebugRuntimeBreakpointSetValidate(const UmiDebugRuntimeCapabilities *capabilities,
    const UmiDebugBreakpointSnapshot *items, size_t count);
/** Publish exactly one reply per enabled requested row, in request order.
 * Missing/extra replies are PARSE_ERROR. A changed registry is BUSY.
 * requested must contain enabled records copied at expectedRevision on the
 * owner thread. Validation/allocation failure changes nothing. Disabled and
 * other-source rows remain untouched. Adapter-adjusted locations are accepted;
 * source identity and user properties remain owned by the requested records. */
UmiStatus UmiDebugRuntimeApplyBreakpointReply(UmiDebugBreakpointRegistry *registry,
    uint64_t expectedRevision, const UmiDebugBreakpointSnapshot *requested, size_t count,
    const UmiDebugRuntimeBreakpointList *reply);
#ifdef __cplusplus
}
#endif
#endif
