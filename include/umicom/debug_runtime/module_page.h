/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/module_page.h
 * PURPOSE: Read bounded module pages tied to the current debugger connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_RUNTIME_MODULE_PAGE_H
#define UMICOM_DEBUG_RUNTIME_MODULE_PAGE_H
#include "umicom/debug_runtime/platform.h"
#ifdef __cplusplus
extern "C"
{
#endif
    enum
    {
        UMI_DEBUG_MODULE_PAGE_LIMIT = 32
    };
    typedef struct UmiDebugModuleSession
    {
        char session_id[128];
        uint64_t generation;
        int supported;
    } UmiDebugModuleSession;
    typedef struct UmiDebugModuleRecord
    {
        int numeric_id;
        int64_t number;
        char id[192];
        char name[256];
        char path[2048];
        char version[128];
        char symbol_status[256];
        char symbol_path[2048];
        char timestamp[128];
        char address_range[256];
        int optimized_known;
        int optimized;
        int user_code_known;
        int user_code;
    } UmiDebugModuleRecord;
    typedef struct UmiDebugModulePage
    {
        UmiDebugModuleSession session;
        uint32_t first;
        uint32_t requested_count;
        size_t count;
        uint64_t total;
        int total_known;
        int has_more;
        UmiDebugModuleRecord items[UMI_DEBUG_MODULE_PAGE_LIMIT];
    } UmiDebugModulePage;
    /** Decode one complete modules response without contacting a debugger.
 * first is at most INT32_MAX; requested_count is 1..UMI_DEBUG_MODULE_PAGE_LIMIT.
 * Required IDs retain their numeric or string type. Known text fields must fit
 * without truncation and contain valid non-NUL UTF-8; duplicate IDs are refused.
 * A present total must agree with the returned range. Without a total, a full
 * page means another explicit request may be needed to discover the end.
 * Failure preserves out. Successful decoding leaves session identity empty. */
    UmiStatus UmiDebugModulePageDecode(const char *json, uint32_t first, uint32_t requested_count,
                                       UmiDebugModulePage *out);
    /** Copy the current active connection identity and negotiated module capability.
 * This call performs no adapter I/O and returns NOT_FOUND when disconnected.
 * Generation changes on initialization and restart attempts, even if a host reuses
 * a session name. The caller owns the copy and must not edit its identity. */
    UmiStatus UmiDebugRuntimeModuleSessionRead(const UmiDebugRuntimePlatform *platform,
                                               UmiDebugModuleSession *out);
    /** Capture one page for exactly the supplied active connection.
 * Call on the platform's owner thread after authorizing the current workspace.
 * Performs one bounded synchronous modules request, with no source loading,
 * file execution, expression evaluation or registry replacement.
 * Adapter paths remain metadata; they are not trusted local file identities.
 * Pages are observations at different times, not a frozen process inventory.
 * Output changes only on success. No automatic retry or background poll occurs. */
    UmiStatus UmiDebugRuntimeModulePageRead(UmiDebugRuntimePlatform *platform,
                                            const UmiDebugModuleSession *session, uint32_t first,
                                            uint32_t requested_count, uint32_t timeout_ms,
                                            UmiDebugModulePage *out);
#ifdef __cplusplus
}
#endif
#endif
