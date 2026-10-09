/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/function_breakpoint_session.h
 * PURPOSE: Bind function-name breakpoint edits and verification to an active debugger session.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_FUNCTION_BREAKPOINT_SESSION_H
#define UMICOM_DEBUG_RUNTIME_FUNCTION_BREAKPOINT_SESSION_H
#include "umicom/debug_runtime/function_breakpoint.h"
#include "umicom/debug_runtime/platform.h"
#ifdef __cplusplus
extern "C"
{
#endif
    enum
    {
        UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT = 32
    };
    typedef struct UmiDebugFunctionEntry
    {
        UmiDebugRuntimeFunctionBreakpoint breakpoint;
        int enabled;
    } UmiDebugFunctionEntry;
    typedef struct UmiDebugFunctionDraft
    {
        char session_id[128];
        uint64_t revision;
        size_t count;
        UmiDebugFunctionEntry entries[UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT];
    } UmiDebugFunctionDraft;
    typedef struct UmiDebugFunctionVerification
    {
        int verified;
        uint64_t adapter_id;
        uint32_t line;
        uint32_t column;
        char source[2048];
        char message[1024];
    } UmiDebugFunctionVerification;
    typedef struct UmiDebugFunctionReply
    {
        size_t count;
        UmiDebugFunctionVerification entries[UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT];
    } UmiDebugFunctionReply;
    typedef struct UmiDebugFunctionSnapshot
    {
        UmiDebugFunctionDraft draft;
        UmiDebugFunctionReply reply;
        int supported;
        int conditions_supported;
        int hit_conditions_supported;
        int acknowledged;
    } UmiDebugFunctionSnapshot;
    /** Validate a complete draft without executing an adapter or modifying state.
 * Names must be nonempty, unique, bounded UTF-8 without control characters.
 * Optional conditions and hit counts are bounded UTF-8 strings. All fixed arrays
 * must contain terminators; enabled values must be 0 or 1. Count zero is valid. */
    UmiStatus UmiDebugFunctionDraftValidate(const UmiDebugFunctionDraft *draft);
    /** Decode a complete setFunctionBreakpoints response into bounded verification.
 * A successful response must contain exactly expected_count breakpoint entries.
 * Unknown metadata is ignored, but malformed or oversized known fields fail
 * without replacing out. Verification describes this response, not later events. */
    UmiStatus UmiDebugFunctionReplyDecode(const char *json, size_t expected_count,
                                          UmiDebugFunctionReply *out);
    /** Read the active session's requested function breakpoints and last Apply result.
 * Fresh sessions begin with an empty, unacknowledged draft; no request is sent
 * merely by reading. Returned records are owned copies. No active session returns
 * NOT_FOUND. Choices are not carried into another session or saved to disk. */
    UmiStatus UmiDebugRuntimeFunctionBreakpointsRead(const UmiDebugRuntimePlatform *platform,
                                                     UmiDebugFunctionSnapshot *out);
    /** Replace all enabled function breakpoints after checking session and revision.
 * Disabled entries remain in the local draft but are omitted from the request;
 * an empty enabled set explicitly clears adapter function breakpoints.
 * Adapter capabilities gate function, condition and hit-condition support.
 * Conditions are debugger expressions and may affect the target; hosts obtain
 * explicit user authorization. This call does not continue or terminate it.
 * A sent attempt advances revision and invalidates earlier acknowledgements even
 * on failure. Read again after an error; retry only by explicit user action.
 * Call on the platform owner thread with a bounded nonzero timeout. */
    UmiStatus UmiDebugRuntimeFunctionBreakpointsApply(UmiDebugRuntimePlatform *platform,
                                                      const UmiDebugFunctionDraft *draft,
                                                      uint32_t timeout_ms);
#ifdef __cplusplus
}
#endif
#endif
