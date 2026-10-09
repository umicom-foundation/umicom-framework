/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/source_catalog.h
 * PURPOSE: Inspect adapter-owned source references without treating remote paths as local files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_SOURCE_CATALOG_H
#define UMICOM_DEBUG_RUNTIME_SOURCE_CATALOG_H
#include "umicom/debug_runtime/connection_identity.h"
#ifdef __cplusplus
extern "C"
{
#endif
    enum
    {
        UMI_DEBUG_SOURCE_CATALOG_LIMIT = 128,
        UMI_DEBUG_SOURCE_CONTENT_CAPACITY = 60000
    };
    typedef struct UmiDebugSourceRecord
    {
        char name[256];
        char path[2048];
        char origin[256];
        char presentation_hint[32];
        uint32_t reference;
        size_t related_count;
    } UmiDebugSourceRecord;
    typedef struct UmiDebugSourceCatalog
    {
        UmiDebugConnectionIdentity connection;
        size_t count;
        UmiDebugSourceRecord items[UMI_DEBUG_SOURCE_CATALOG_LIMIT];
    } UmiDebugSourceCatalog;
    typedef struct UmiDebugSourceContent
    {
        char mime_type[128];
        char text[UMI_DEBUG_SOURCE_CONTENT_CAPACITY];
        size_t bytes;
    } UmiDebugSourceContent;
    /** Decode a complete loadedSources response into caller-owned heap storage.
 * Top-level records are bounded; nested related sources are counted, not expanded.
 * Optional strings remain empty when absent. References must fit nonnegative
 * int32; zero means the adapter did not provide a retrievable source handle.
 * Oversized catalogues and fields fail without truncating or changing out. */
    UmiStatus UmiDebugSourceCatalogDecode(const char *json, UmiDebugSourceCatalog *out);
    /** Decode plain source text and its optional MIME type without rendering markup.
 * Text is valid non-NUL UTF-8 and may be empty. bytes excludes its terminator.
 * Failure preserves out. Allocate this large value outside the thread stack. */
    UmiStatus UmiDebugSourceContentDecode(const char *json, UmiDebugSourceContent *out);
    /** Read the current adapter's complete bounded source catalogue.
 * Call on the platform owner thread after authorizing the current workspace.
 * Requires an active connection advertising loadedSources support. Performs one
 * synchronous bounded request; no disk read, expression evaluation or retry.
 * On success the catalogue owns a captured connection identity; failure preserves out. */
    UmiStatus UmiDebugRuntimeSourceCatalogRead(UmiDebugRuntimePlatform *platform,
                                               uint32_t timeout_ms, UmiDebugSourceCatalog *out);
    /** Request text for a positive source reference on exactly the captured connection.
 * References are 1..INT32_MAX and must come from the adapter's source metadata.
 * Paths are deliberately absent from this interface: only the adapter resolves
 * this handle. No local file is read or written. The returned text is an observation
 * at request time, not a claim that the debugged program matches a file on disk.
 * Failure preserves out, and no automatic retry occurs. */
    UmiStatus UmiDebugRuntimeSourceContentRead(UmiDebugRuntimePlatform *platform,
                                               const UmiDebugConnectionIdentity *connection,
                                               uint32_t reference, uint32_t timeout_ms,
                                               UmiDebugSourceContent *out);
#ifdef __cplusplus
}
#endif
#endif
