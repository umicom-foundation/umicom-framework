/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/exception_filters.h
 * PURPOSE: Expose the connected adapter exception filters with session-bound selection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_EXCEPTION_FILTERS_H
#define UMICOM_DEBUG_RUNTIME_EXCEPTION_FILTERS_H
#include "umicom/debug_runtime/platform.h"
#ifdef __cplusplus
extern "C"
{
#endif
    enum
    {
        UMI_DEBUG_EXCEPTION_FILTER_LIMIT = 32
    };
    /* IDs are protocol values supplied by the adapter; labels are presentation.
 * Keep them separate so translated labels never become requests. */
    typedef struct UmiDebugExceptionFilter
    {
        char id[128];
        char label[256];
        char description[768];
        int default_enabled;
    } UmiDebugExceptionFilter;
    typedef struct UmiDebugExceptionCatalog
    {
        size_t count;
        UmiDebugExceptionFilter items[UMI_DEBUG_EXCEPTION_FILTER_LIMIT];
    } UmiDebugExceptionCatalog;
    typedef struct UmiDebugExceptionSelection
    {
        char session_id[128];
        uint64_t revision;
        size_t count;
        int enabled[UMI_DEBUG_EXCEPTION_FILTER_LIMIT];
    } UmiDebugExceptionSelection;
    typedef struct UmiDebugExceptionAcknowledgement
    {
        size_t count;
        /* -1 means the adapter omitted per-filter verification, 0 refused, 1 verified. */
        int verified[UMI_DEBUG_EXCEPTION_FILTER_LIMIT];
        char message[UMI_DEBUG_EXCEPTION_FILTER_LIMIT][512];
    } UmiDebugExceptionAcknowledgement;
    /* The original aggregate reused the persisted exception-record name from
     * debug/exception.h. That declaration is retained below for engineering review.
     * UmiDebugExceptionFiltersSnapshot gives session filter state its own public
     * identity while preserving all fields, their order and Framework ownership.
     * Callers of filter APIs must use this aggregate; UmiDebugExceptionSnapshot
     * continues to name the established record used by registries and archives. */
#if 0
    typedef struct UmiDebugExceptionSnapshot
    {
        UmiDebugExceptionCatalog catalog;
        UmiDebugExceptionSelection selection;
        UmiDebugExceptionAcknowledgement acknowledgement;
        int acknowledged;
    } UmiDebugExceptionSnapshot;
#endif
    typedef struct UmiDebugExceptionFiltersSnapshot
    {
        UmiDebugExceptionCatalog catalog;
        UmiDebugExceptionSelection selection;
        UmiDebugExceptionAcknowledgement acknowledgement;
        int acknowledged;
    } UmiDebugExceptionFiltersSnapshot;
    /** Read exceptionBreakpointFilters from a complete successful initialize response.
 * Required IDs and labels must be unique (IDs), bounded, valid non-NUL UTF-8.
 * Missing filters mean an empty catalogue. Unknown optional metadata is ignored;
 * conditions are not part of this basic filter-selection operation.
 * Failure preserves out. Parsing never contacts an adapter. */
    UmiStatus UmiDebugExceptionCatalogDecode(const char *json, UmiDebugExceptionCatalog *out);
    /** Decode an optional setExceptionBreakpoints response body for the selected filters.
 * A missing body or breakpoint array leaves verification unknown. A present array
 * must match selected_count exactly; malformed entries preserve out unchanged. */
    UmiStatus UmiDebugExceptionAcknowledgementDecode(const char *json, size_t selected_count,
                                                     UmiDebugExceptionAcknowledgement *out);
    /** Copy the current active session's exception catalogue and last requested selection.
 * Session ID and revision bind a later Apply to this exact state. Before an active
 * session this returns NOT_FOUND; zero filters means this adapter did not advertise any.
 * acknowledged means a successful protocol response, not that every filter verified. */
    /* Retain the superseded declaration for review. The function name and
     * behaviour stay stable; its output now names the correct filter aggregate
     * so callers cannot allocate the smaller persisted exception record. */
#if 0
    UmiStatus UmiDebugRuntimeExceptionFiltersRead(const UmiDebugRuntimePlatform *platform,
                                                  UmiDebugExceptionSnapshot *out);
#endif
    UmiStatus UmiDebugRuntimeExceptionFiltersRead(const UmiDebugRuntimePlatform *platform,
                                                  UmiDebugExceptionFiltersSnapshot *out);
    /** Replace the complete selection for the same active session and revision.
 * The caller explicitly authorizes this change and keeps the platform on its owner
 * thread. Only 0/1 values for advertised entries are accepted. No condition,
 * expression evaluation, resume or target termination request is sent.
 * A sent request advances revision even on failure. Read again after an error;
 * acknowledgement is then unknown, and no automatic retry is performed.
 * This synchronous operation uses a bounded timeout and never stores choices on disk. */
    UmiStatus UmiDebugRuntimeExceptionFiltersApply(UmiDebugRuntimePlatform *platform,
                                                   const UmiDebugExceptionSelection *selection,
                                                   uint32_t timeout_ms);
#ifdef __cplusplus
}
#endif
#endif
