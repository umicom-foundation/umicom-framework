/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/job_history.h
 * PURPOSE: Keep bounded job outcomes in the shared Data Server without storing credentials.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DATA_JOB_HISTORY_H
#define UMICOM_DATA_JOB_HISTORY_H
#include "umicom/data/data_server.h"
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif

#define UMI_JOB_HISTORY_CAPACITY 64U
#define UMI_JOB_HISTORY_KIND_CAPACITY 65U
#define UMI_JOB_HISTORY_LABEL_CAPACITY 129U
    typedef struct UmiJobHistory UmiJobHistory;
    typedef enum UmiJobHistoryState
    {
        UMI_JOB_HISTORY_PREPARED = 1,
        UMI_JOB_HISTORY_RUNNING = 2,
        UMI_JOB_HISTORY_SUCCEEDED = 3,
        UMI_JOB_HISTORY_FAILED = 4,
        UMI_JOB_HISTORY_CANCELLED = 5
    } UmiJobHistoryState;
    typedef struct UmiJobHistoryEntry
    {
        uint64_t id;
        uint64_t revision;
        UmiJobHistoryState state;
        unsigned completed_steps;
        unsigned total_steps;
        UmiStatus result;
        char kind[UMI_JOB_HISTORY_KIND_CAPACITY];
        char label[UMI_JOB_HISTORY_LABEL_CAPACITY];
    } UmiJobHistoryEntry;
    typedef struct UmiJobHistorySnapshot
    {
        size_t count;
        size_t unfinished_count;
        UmiJobHistoryEntry entries[UMI_JOB_HISTORY_CAPACITY];
    } UmiJobHistorySnapshot;

    /* The history borrows its server; join workers before destroying either object.
 * scope is a stable ASCII identifier (letters, digits, dot, dash, underscore).
 * Each call owns a short transaction. Do not call inside an existing transaction.
 * SQLite provides persistence; a memory server is useful for transient hosts.
 * Metadata is not a secret store: never supply keys, prompts, signed URLs, account
 * numbers, command lines or output. The label is a short, non-sensitive caption.
 * Opening and capturing history never schedules work or changes old outcomes. */
    UmiStatus UmiJobHistoryCreate(UmiDataServer *server, const char *scope, UmiJobHistory **out_history);
    void UmiJobHistoryDestroy(UmiJobHistory *history);
    /* Allocate a durable, never-reused identifier before starting an external action.
 * Full history refuses new work; it never silently evicts unfinished evidence. */
    UmiStatus UmiJobHistoryBegin(UmiJobHistory *history, const char *kind, const char *label,
                                 unsigned total_steps, UmiJobHistoryEntry *out_entry);
    /* Compare the persisted revision before replacing it. Progress cannot move back.
 * PREPARED -> RUNNING -> terminal; PREPARED may fail/cancel before execution.
 * Success requires every step. Failure/cancellation retain actual progress.
 * Output values remain unchanged when a call fails, including a commit failure. */
    UmiStatus UmiJobHistoryUpdate(UmiJobHistory *history, uint64_t id, uint64_t expected_revision,
                                  UmiJobHistoryState state, unsigned completed_steps, UmiStatus result,
                                  UmiJobHistoryEntry *out_entry);
    /* Sorted by identifier. PREPARED/RUNNING from another session mean "outcome
 * unknown", not proof that a process is still running. Never replay them.
 * Allocate snapshots on the heap where frontend stack space is limited. */
    UmiStatus UmiJobHistoryCapture(UmiJobHistory *history, UmiJobHistorySnapshot *out_snapshot);
    /* Explicit retention action: atomically remove only terminal entries.
 * Unfinished entries and the monotonic identifier are retained. */
    UmiStatus UmiJobHistoryPruneFinished(UmiJobHistory *history, size_t *out_removed);
    bool UmiJobHistoryIsFinished(UmiJobHistoryState state);
    const char *UmiJobHistoryStateText(UmiJobHistoryState state);
#ifdef __cplusplus
}
#endif
#endif
