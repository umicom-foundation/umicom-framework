/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/search_session.h
 *
 * PURPOSE:
 *   Run saved-file searches through the existing Framework task queue and search
 *   engine, retaining bounded results without exposing worker-owned memory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_SEARCH_SESSION_H
#define UMICOM_PLATFORM_SEARCH_SESSION_H

#include "umicom/platform/search.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FILE_SEARCH_QUERY_CAPACITY 1024U
#define UMI_FILE_SEARCH_MAX_RESULTS 1000U
#define UMI_FILE_SEARCH_MAX_FILE_BYTES (8U * 1024U * 1024U)

typedef struct UmiFileSearchSession UmiFileSearchSession;

/** Copied state. active includes queue finalisation. Results become available
 * only after successful completion; counts during work are progress, not a
 * complete result set. requestId zero means no search has been submitted. */
typedef struct UmiFileSearchSnapshot {
    char query[UMI_FILE_SEARCH_QUERY_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    uint64_t requestId;
    uint64_t indexRevision;
    UmiSearchStats stats;
    UmiStatus status;
    int active;
    int ready;
    int stale;
    int caseSensitive;
} UmiFileSearchSnapshot;

/** Borrow index until destruction completes. Call lifecycle and result APIs on
 * one owner thread. The worker uses copied input and never calls a UI callback.
 * Destroy the session before its index; destroy may wait for a blocking OS read.
 * This is saved-file search, not search of unsaved editor drafts or a sandbox. */
UmiStatus UmiFileSearchCreate(const UmiFileIndex *index,
    UmiFileSearchSession **outSession);
void UmiFileSearchDestroy(UmiFileSearchSession *session);

/** Start a literal query on one lazy worker. At most one request is outstanding;
 * BUSY never queues another. A query has 1..1023 bytes, no CR/LF, and is copied.
 * expectedRevision zero captures the current index. The root must be absolute.
 * A successful call means accepted, not completed. Invalid requests preserve
 * earlier results. Case-insensitive search folds ASCII letters only. */
UmiStatus UmiFileSearchStart(UmiFileSearchSession *session, const char *query,
    int caseSensitive, uint64_t expectedRevision);
/** Non-blocking cooperative cancellation; an already completed result remains. */
UmiStatus UmiFileSearchCancel(UmiFileSearchSession *session);
/** Invalidate results and request cancellation when a host changes workspace. */
void UmiFileSearchInvalidate(UmiFileSearchSession *session);
/** Read current status. An index revision change invalidates retained results. */
UmiStatus UmiFileSearchRead(UmiFileSearchSession *session,
    UmiFileSearchSnapshot *outSnapshot);
/** Copy one match from this successful request, rejecting stale row identities.
 * No output is changed on failure. The path and position refer to saved bytes
 * at search time; opening a file must not overwrite an existing unsaved draft. */
UmiStatus UmiFileSearchMatchAt(UmiFileSearchSession *session, uint64_t requestId,
    size_t position, UmiSearchMatch *outMatch);
/** Wait for the owned queue (not for an external build). Zero waits indefinitely.
 * This helper is for tests/CLI/shutdown, never a GUI event handler. Read status
 * afterwards: waiting successfully does not mean the search succeeded. */
UmiStatus UmiFileSearchWait(UmiFileSearchSession *session,
    uint32_t timeoutMilliseconds);

#ifdef __cplusplus
}
#endif
#endif
