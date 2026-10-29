/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_progress.h
 *
 * PURPOSE:
 *   Keep local checkpoint history bound to verified lesson revisions and bytes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_LEARNING_PROGRESS_H
#define UMICOM_TEACHER_LEARNING_PROGRESS_H
#include "umicom/teacher/learning_reader.h"
#include "umicom/data/data_server.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LEARNING_PROGRESS_MAX_EVENTS 256U
typedef struct UmiLearningProgress UmiLearningProgress;
typedef struct UmiLearningProgressEvent {
    uint64_t sequence;
    char pack_id[UMI_TEACHER_ID_CAPACITY];
    uint64_t pack_revision;
    char resource_id[UMI_TEACHER_ID_CAPACITY];
    char sha256[UMI_SHA256_HEX_CAPACITY];
    char checkpoint[UMI_TEACHER_ID_CAPACITY];
    char evidence_reference[UMI_TEACHER_ID_CAPACITY];
} UmiLearningProgressEvent;
typedef struct UmiLearningProgressSnapshot {
    char learner_id[UMI_TEACHER_ID_CAPACITY];
    size_t event_count;
    UmiDataServerBackend backend;
    int recovery_required;
} UmiLearningProgressSnapshot;

/* Borrow an explicitly selected Data Server for one local learner. Keep it
 * alive until Close, and serialize all calls on this owner. Open reads an
 * existing journal in a transaction; it never creates progress or selects a
 * filesystem location. The server must not already be in a transaction.
 * Memory storage is temporary; the SQLite backend can reopen saved records.
 * No fallback between those backends takes place.
 *
 * IDs are nonempty ASCII letters/digits followed by letters/digits, '.', '-'
 * or '_', at most UMI_TEACHER_ID_CAPACITY-1 bytes. Input pointers must reference
 * terminated C strings. Invalid/corrupt/unknown records are refused, not reset.
 * Failure clears out_progress. The caller owns any existing value in that slot.
 * This local journal is plaintext and does not authenticate a learner, teacher
 * or evidence. Use a separate location from source packs and AI conversations. */
UmiStatus UmiLearningProgressOpen(UmiDataServer *server, const char *learner_id,
    UmiLearningProgress **out_progress);
void UmiLearningProgressClose(UmiLearningProgress *progress);

/* Refresh only after an ordinary stale-writer BUSY refusal. Read/parse failures
 * preserve the old projection. After commit/rollback uncertainty this owner is
 * retired for writes; close it and reopen with a newly established connection.
 * A snapshot still exposes the last known records and recovery_required. */
UmiStatus UmiLearningProgressRefresh(UmiLearningProgress *progress);
UmiStatus UmiLearningProgressCopySnapshot(const UmiLearningProgress *progress,
    UmiLearningProgressSnapshot *out);
UmiStatus UmiLearningProgressEventAt(const UmiLearningProgress *progress, size_t index,
    UmiLearningProgressEvent *out);

/* Append an explicit checkpoint for the exact verified document in reader.
 * Only GUIDE and LESSON are supported. Merely opening or reading a document
 * never calls this function. Checkpoint names and opaque evidence references
 * are application policy; this layer does not grade, grant mastery, verify an
 * evidence file or run an example. Do not put personal text, paths or secrets
 * into those identifiers.
 *
 * Reload the journal inside a transaction and compare its complete canonical
 * contents with this owner's projection before writing. Another writer or
 * changed history returns BUSY without merging. Exact duplicate events return
 * ALREADY_EXISTS. A different revision/digest is a separate event. No existing
 * event is edited or deleted; reaching 256 events returns CAPACITY_EXCEEDED.
 * Storage failures preserve out_event and the last known projection; uncertain
 * commit/rollback retires further writes. Do not silently retry a failed write.
 * The reader is borrowed only for this call and must not change concurrently. */
UmiStatus UmiLearningProgressRecord(UmiLearningProgress *progress,
    const UmiLearningReader *reader, const char *checkpoint, const char *evidence_reference,
    UmiLearningProgressEvent *out_event);

/* Find the latest checkpoint for this exact pack ID/revision, resource ID and
 * digest. Prior lesson versions remain visible through EventAt but cannot
 * satisfy this query. No match returns NOT_FOUND. Refusals preserve outputs.
 * Queries use the current in-memory projection, so Refresh before comparing
 * with work written through another owner. Recovery-required owners refuse
 * this current-progress query rather than presenting uncertain state as current. */
UmiStatus UmiLearningProgressFindCurrent(const UmiLearningProgress *progress,
    const UmiLearningReader *reader, const char *checkpoint, UmiLearningProgressEvent *out);


#define UMI_LEARNING_PROGRESS_MAX_ARCHIVE_BYTES (UMI_LEARNING_PROGRESS_MAX_EVENTS * 641U + 256U)
/* Export a portable UTF-8 text archive of this owner's current projection.
 * The archive contains the local learner ID and evidence references: treat it
 * as private data, never a public course asset. It is not signed or encrypted.
 * No filesystem operation or implicit refresh occurs. Uncertain owners refuse.
 *
 * out_size excludes the final NUL. NULL/0 queries the required size and returns
 * CAPACITY_EXCEEDED. Short buffers report that size without changing bytes.
 * Other refusals preserve both outputs. The caller chooses where to save it. */
UmiStatus UmiLearningProgressExport(const UmiLearningProgress *progress,
    void *out_bytes, size_t capacity, size_t *out_size);

/* Import into an EMPTY journal for the SAME learner ID. Existing records are
 * never overwritten, merged or relabelled: return ALREADY_EXISTS instead.
 * Validate the complete bounded archive before opening a transaction. Unknown
 * format versions, NUL bytes, missing final newline, malformed/duplicate events
 * and extra data are refused. LF and CRLF are accepted; export always uses LF.
 * Empty archives are valid and do not create stored records.
 *
 * A valid archive is self-reported local history, not evidence authentication.
 * All events are committed together. Commit/rollback uncertainty retires this
 * owner as for Record. It must be explicitly selected by the application; no
 * file scan, automatic synchronisation or course-directory write takes place. */
UmiStatus UmiLearningProgressImportEmpty(UmiLearningProgress *progress,
    const void *archive, size_t size);

#ifdef __cplusplus
}
#endif
#endif
