/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Recovery snapshots retain both the reviewed record and unsaved bytes. They
 * are independent of a window, a database connection and a pending worker. */
#ifndef UMICOM_DATA_RECORD_DRAFT_SNAPSHOT_H
#define UMICOM_DATA_RECORD_DRAFT_SNAPSHOT_H
#include "umicom/data/record_draft.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RECORD_DRAFT_SNAPSHOT_MAX_BYTES (24U + UMI_RECORD_REVISION_TEXT_CAPACITY + 2U * UMI_RECORD_DOCUMENT_MAX_BYTES + 64U)
typedef struct UmiRecordDraftSnapshot {
    UmiRecordDocument baseline;
    size_t length;
    unsigned char content[UMI_RECORD_DOCUMENT_MAX_BYTES];
} UmiRecordDraftSnapshot;

/* Capture a ready draft on its owning thread. A pending save returns BUSY:
 * recovery must not guess whether an unfinished write reached the database.
 * The snapshot copies all meaningful bytes and zeroes unused storage. It does
 * not save the record or clear dirty state. Outputs stay unchanged on failure. */
UmiStatus umi_record_draft_snapshot_capture(const UmiRecordDraft *draft, UmiRecordDraftSnapshot *output);
UmiStatus umi_record_draft_snapshot_validate(const UmiRecordDraftSnapshot *snapshot);
/* Explicitly restore into an empty or clean draft; dirty content and pending
 * requests are never replaced. Keep the original reviewed revision, even when
 * newer revisions exist. The next save still performs its normal stale check.
 * Request IDs remain monotonic for the receiving draft's entire lifetime. */
UmiStatus umi_record_draft_snapshot_restore(UmiRecordDraft *draft, const UmiRecordDraftSnapshot *snapshot);
/* Before restoring against a selected store, verify that its historical
 * baseline has the same canonical metadata and content. A new draft has no
 * stored baseline to compare; a collision is refused by its later first save.
 * This does not authenticate a database or lock its head until the next save. */
UmiStatus umi_record_draft_snapshot_check_store(UmiDataServer *server, const UmiRecordDraftSnapshot *snapshot);

/* A versioned, length-framed binary codec; arbitrary bytes including NUL are
 * retained. A SHA-256 covers framing, metadata and both contents. This detects
 * accidental corruption, not tampering by a writer able to replace the file.
 * No pointer, struct padding, request token or unused buffer tail is encoded.
 * Encode allocates output, freed with BytesFree. Decode validates the complete
 * bounded input before publishing output. All errors leave outputs unchanged. */
UmiStatus umi_record_draft_snapshot_encode(const UmiRecordDraftSnapshot *snapshot, unsigned char **output, size_t *output_length);
UmiStatus umi_record_draft_snapshot_decode(const void *bytes, size_t length, UmiRecordDraftSnapshot *output);
void umi_record_draft_snapshot_bytes_free(void *bytes);
#ifdef __cplusplus
}
#endif
#endif
