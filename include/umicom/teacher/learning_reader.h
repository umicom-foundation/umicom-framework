/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_reader.h
 *
 * PURPOSE:
 *   Own verified lesson bytes and bounded navigation history for product readers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_LEARNING_READER_H
#define UMICOM_TEACHER_LEARNING_READER_H
#include "umicom/teacher/learning_navigation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LEARNING_READER_HISTORY_CAPACITY 64U
#define UMI_LEARNING_READER_MAX_BYTES (16U * 1024U * 1024U)

typedef struct UmiLearningReader UmiLearningReader;
typedef struct UmiLearningReaderSnapshot {
    UmiLearningPackInfo pack;
    UmiLearningPackLink selection;
    size_t content_size;
    size_t history_count;
    size_t history_index;
    uint64_t revision;
    int has_document;
    int can_go_back;
    int can_go_forward;
} UmiLearningReaderSnapshot;

/* Transfer one immutable pack into a reader only on success, clearing *pack.
 * maximum_bytes must be 1..16 MiB. All reader calls are serialized by the owner.
 * Opening/reading can block: use an application worker, then transfer copied
 * snapshots/content to the UI. The reader never runs a renderer or source code.
 * Adoption is not permission to compile, execute, download or trust a project. */
UmiStatus UmiLearningReaderCreate(UmiLearningPack **pack, size_t maximum_bytes,
    UmiLearningReader **out_reader);
void UmiLearningReaderDestroy(UmiLearningReader *reader);

/* Open a copied catalogue selection in a new, independent reader. Supply the
 * pack revision and lowercase SHA-256 from the displayed search result. A
 * replaced pack or changed resource digest returns INVALID_STATE; a removed
 * pack/resource returns NOT_FOUND. Verify file bytes before publishing the
 * reader. Failure clears *out_reader and leaves the shelf unchanged.
 * expected_revision must be positive and expected_sha256 must contain exactly
 * 64 lowercase hexadecimal characters. Serialize shelf access for this call,
 * which may block on reading. The result does not depend on shelf lifetime.
 * Revision/digest checks detect a stale selection, not publisher authenticity;
 * applications still decide which pack roots and manifests they admit. */
UmiStatus UmiLearningReaderCreateFromShelf(const UmiLearningShelf *shelf,
    const char *pack_id, uint64_t expected_revision, const char *resource_id,
    const char *expected_sha256, size_t maximum_bytes, UmiLearningReader **out_reader);

/* Open an explicit resource ID or follow this pack's restricted local-link
 * profile. Read and verify the candidate bytes before replacing the current
 * document or history. Failure preserves the previous selection and content.
 * Successful new navigation discards the forward branch. At 64 entries, the
 * oldest entry is evicted. Reopening the current selection rechecks its bytes
 * without duplicating history or discarding a forward branch. */
UmiStatus UmiLearningReaderOpen(UmiLearningReader *reader, const char *resource_id);
/* Open another result in this reader while retaining its history. The supplied
 * pack ID/revision and resource digest must match this reader's admitted pack.
 * A mismatch returns INVALID_STATE; a missing resource returns NOT_FOUND.
 * Failure preserves selection, bytes and history. All string arguments must
 * be valid terminated strings. The digest must exactly match its manifest. */
UmiStatus UmiLearningReaderOpenSelection(UmiLearningReader *reader, const char *pack_id,
    uint64_t expected_revision, const char *resource_id, const char *expected_sha256);
UmiStatus UmiLearningReaderFollow(UmiLearningReader *reader, const char *local_link);
UmiStatus UmiLearningReaderBack(UmiLearningReader *reader);
UmiStatus UmiLearningReaderForward(UmiLearningReader *reader);
UmiStatus UmiLearningReaderReload(UmiLearningReader *reader);

/* Copy metadata independently of reader lifetime. An empty reader has zero
 * history and no current document. Refusal leaves the snapshot unchanged. */
UmiStatus UmiLearningReaderCopySnapshot(const UmiLearningReader *reader,
    UmiLearningReaderSnapshot *out);

/* Copy already verified bytes; do not reopen a path. On OK, *out_size excludes
 * the extra terminating NUL. Binary content is retained, so use the byte count.
 * On insufficient capacity, return CAPACITY_EXCEEDED and the required byte
 * count while preserving the destination. NULL/0 queries the byte count.
 * Allocate required+1 bytes for a subsequent copy. The application controls
 * rendering, UTF-8 admission and image decoding. No markup is executed here. */
UmiStatus UmiLearningReaderCopyContent(const UmiLearningReader *reader,
    void *out_bytes, size_t capacity, size_t *out_size);

#ifdef __cplusplus
}
#endif
#endif
