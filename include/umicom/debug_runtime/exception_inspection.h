/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/exception_inspection.h
 * PURPOSE: Inspect the exception from an exact stopped thread without evaluating adapter expressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_EXCEPTION_INSPECTION_H
#define UMICOM_DEBUG_RUNTIME_EXCEPTION_INSPECTION_H
#include "umicom/debug_runtime/connection_identity.h"
#ifdef __cplusplus
extern "C"
{
#endif
    enum
    {
        UMI_DEBUG_EXCEPTION_DETAIL_LIMIT = 32,
        UMI_DEBUG_EXCEPTION_DEPTH_LIMIT = 8
    };
    typedef struct UmiDebugExceptionTarget
    {
        UmiDebugConnectionIdentity connection;
        uint64_t revision;
        uint32_t thread_id;
    } UmiDebugExceptionTarget;
    typedef struct UmiDebugExceptionDetail
    {
        size_t parent;
        unsigned depth;
        char message[2048];
        char type_name[512];
        char full_type_name[1024];
        char evaluate_name[1024];
        char stack_trace[8192];
    } UmiDebugExceptionDetail;
    typedef struct UmiDebugExceptionInformation
    {
        char exception_id[512];
        char description[2048];
        char break_mode[64];
        size_t count;
        UmiDebugExceptionDetail details[UMI_DEBUG_EXCEPTION_DETAIL_LIMIT];
    } UmiDebugExceptionInformation;
    /** Decode exceptionInfo into caller-owned heap storage with complete failure preservation.
 * Inner exceptions retain parent indexes; the root parent is SIZE_MAX.
 * At most 32 details and eight levels are accepted. Known text must be bounded
 * non-NUL UTF-8; no stack trace, expression or markup is interpreted or executed.
 * Unknown break-mode spellings remain descriptive text for adapter compatibility. */
    UmiStatus UmiDebugExceptionInformationDecode(const char *json,
                                                 UmiDebugExceptionInformation *out);
    /** Capture the current native exception stop without contacting the adapter.
 * Requires advertised exception-info support and an observed exception stop.
 * Pending lifecycle events return BUSY. The output remains unchanged on failure.
 * Call on the runtime owner thread after validating the host's workspace authority. */
    UmiStatus UmiDebugRuntimeExceptionTargetRead(const UmiDebugRuntimePlatform *platform,
                                                 UmiDebugExceptionTarget *out);
    /** Read exception details for exactly the captured connection, revision and thread.
 * Performs one bounded synchronous request and checks the target again after waiting.
 * Continued, stopped or exited events queued during the wait prevent publication.
 * Failure preserves out. No evaluation, resume, automatic retry or disk access occurs. */
    UmiStatus UmiDebugRuntimeExceptionInformationRead(UmiDebugRuntimePlatform *platform,
                                                      const UmiDebugExceptionTarget *target,
                                                      uint32_t timeout_ms,
                                                      UmiDebugExceptionInformation *out);
#ifdef __cplusplus
}
#endif
#endif
