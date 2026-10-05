/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/recovery_storage.h
 * PURPOSE: Keep private recovery snapshots independent of source-file names and document lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_RECOVERY_STORAGE_H
#define UMICOM_DOCUMENT_RECOVERY_STORAGE_H
#include "umicom/document/recovery_draft.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DOCUMENT_RECOVERY_CATALOGUE_LIMIT 256U
    typedef struct UmiDocumentRecoveryEntry
    {
        char key[UMI_DOCUMENT_RECOVERY_KEY_CAPACITY];
        uint64_t file_bytes, modified_nanoseconds;
        UmiStatus availability;
    } UmiDocumentRecoveryEntry;
    typedef struct UmiDocumentRecoveryCatalogue UmiDocumentRecoveryCatalogue;
    /* Resolve the per-user recovery directory without creating it. base_override
 * is an absolute test/portable settings base or NULL for native user storage.
 * Both resolver and storage reject dependence on the launch directory. */
    UmiStatus UmiDocumentRecoveryDirectory(const char *application_directory, const char *base_override,
                                           char *out_directory, size_t capacity);
    /* Store a new immutable record in an existing trusted user directory. Creation
 * is exclusive: even the same draft/key cannot replace an existing leaf. Source
 * paths in metadata are never opened. A failed write may leave a partial record
 * for diagnosis; it is never mistaken for successful recovery. No old record is
 * removed, rotated or overwritten. The containing directory and its ancestors
 * must be controlled by the caller; parent renames are not isolated. Calls can
 * block and belong on a worker. Drafts contain plaintext private source. */
    UmiStatus UmiDocumentRecoverySave(const char *directory, const UmiDocumentRecoveryDraft *draft,
                                      const UmiCancellationToken *cancel);
    /* Read one ordinary leaf without following links beneath the directory. Verify
 * the encoded key matches the selected filename, validate the whole record,
 * and publish an owned draft only on success. Failure clears out_draft. */
    UmiStatus UmiDocumentRecoveryLoad(const char *directory, const char *key,
                                      const UmiCancellationToken *cancel,
                                      UmiDocumentRecoveryDraft **out_draft);
    /* Enumerate at most 256 matching names and 4096 directory entries. A missing
 * directory produces an empty catalogue. Overflow, cancellation and I/O errors
 * publish no partial catalogue. Sort newest modification first, then by key.
 * Content is loaded only on explicit selection. availability reports links,
 * non-regular entries and oversized records; OK does not prove decodability. */
    UmiStatus UmiDocumentRecoveryList(const char *directory, const UmiCancellationToken *cancel,
                                      UmiDocumentRecoveryCatalogue **out_catalogue);
    /* Apply an admission budget before an automatic snapshot. Count existing
 * matching records (including unavailable ones) and their file sizes, then
 * refuse a write which would exceed maximum_records or maximum_file_bytes.
 * Nothing is pruned. Serialize callers: this is a checked admission policy,
 * not a cross-process filesystem quota. External writers can race the check. */
    UmiStatus UmiDocumentRecoverySaveWithinBudget(const char *directory,
                                                  const UmiDocumentRecoveryDraft *draft,
                                                  size_t maximum_records, uint64_t maximum_file_bytes,
                                                  const UmiCancellationToken *cancel);
    size_t UmiDocumentRecoveryCatalogueCount(const UmiDocumentRecoveryCatalogue *catalogue);
    UmiStatus UmiDocumentRecoveryCatalogueAt(const UmiDocumentRecoveryCatalogue *catalogue, size_t index,
                                             UmiDocumentRecoveryEntry *out_entry);
    void UmiDocumentRecoveryCatalogueDestroy(UmiDocumentRecoveryCatalogue *catalogue);
#ifdef __cplusplus
}
#endif
#endif
