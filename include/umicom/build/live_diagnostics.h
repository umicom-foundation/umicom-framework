/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/live_diagnostics.h
 * PURPOSE: Expose copied diagnostic counts while a build phase is still running.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_LIVE_DIAGNOSTICS_H
#define UMICOM_BUILD_LIVE_DIAGNOSTICS_H
#include "umicom/build/project_session.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Counts describe records recognised in the current phase, including records
     * beyond the completed-result list capacity. They are not a success verdict.
     * A partial CMake message is counted only when its record ends. */
    typedef struct UmiBuildDiagnosticProgress
    {
        uint64_t operation_id;
        UmiBuildPhase phase;
        size_t phase_index;
        uint64_t errors;
        uint64_t warnings;
        uint64_t notes;
        uint64_t unrepresented;
        bool streamed;
        bool phase_complete;
        bool counters_saturated;
    } UmiBuildDiagnosticProgress;
    /**
     * Copy the current phase's diagnostic progress under the worker mutex. No
     * compiler wait, filesystem I/O or frontend callback occurs. A zero operation
     * id means no accepted work has produced a phase. A legacy executor supplies
     * only its final bounded output and sets streamed=false.
     * Reads may overlap work, but not destruction. Counts saturate explicitly.
     */
    UmiStatus UmiBuildProjectSessionReadDiagnostics(UmiBuildProjectSession *session,
                                                    UmiBuildDiagnosticProgress *out);

    /* A bounded page avoids copying a large retained list on the UI stack.
     * identity and phase accompany every page; first_index is a producer index,
     * never a path or an instruction to open a file. */
#define UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY 16U
    typedef struct UmiBuildDiagnosticPage
    {
        UmiBuildDiagnosticProgress progress;
        char source_directory[UMI_BUILD_PATH_CAPACITY];
        size_t first_index, count, retained_count;
        uint64_t retention_dropped;
        UmiBuildDiagnostic items[UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY];
        /* Captured with the accepted profile, never read from the current UI.
         * Relative compiler paths may originate from source or build tools. */
        char build_directory[UMI_BUILD_PATH_CAPACITY];
    } UmiBuildDiagnosticPage;
    /**
     * Copy one page from the current phase under the session mutex. Zero
     * expected_operation accepts the current phase; otherwise both operation
     * and phase index must still match, or INVALID_STATE leaves out unchanged.
     * A first_index beyond retained_count returns NOT_FOUND unchanged. An index
     * equal to that count returns an empty page with current progress.
     * Retain the first 256 representable records. retention_dropped counts
     * records lost during projection or retention; progress.unrepresented counts
     * malformed/oversized parser records. Neither changes the process outcome.
     * Callers should heap-allocate a page and keep it separate from session
     * storage. Reads may overlap work, but not submission or destruction.
     */
    UmiStatus UmiBuildProjectSessionReadDiagnosticPage(UmiBuildProjectSession *session,
        uint64_t expected_operation, size_t expected_phase_index, size_t first_index,
        UmiBuildDiagnosticPage *out);

#ifdef __cplusplus
}
#endif
#endif
