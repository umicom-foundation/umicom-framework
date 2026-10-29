/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_reader_selection.c
 *
 * PURPOSE:
 *   Recheck a catalogue selection before creating an independent verified reader.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_reader.h"
#include <string.h>

/* Manifest digests use one canonical representation. A bounded scan rejects
 * truncated strings before any following byte is read. The caller supplies a
 * valid C string; this function is not an admission layer for arbitrary pointers. */
static int SelectionDigestValid(const char *digest)
{
    if (digest == NULL) return 0;
    for (size_t i = 0U; i < UMI_SHA256_HEX_CAPACITY - 1U; ++i) {
        char c = digest[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return 0;
    }
    return digest[UMI_SHA256_HEX_CAPACITY - 1U] == '\0';
}

UmiStatus UmiLearningReaderCreateFromShelf(const UmiLearningShelf *shelf,
    const char *pack_id, uint64_t revision, const char *resource_id,
    const char *digest, size_t maximum, UmiLearningReader **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (shelf == NULL || pack_id == NULL || resource_id == NULL || revision == 0U ||
        !SelectionDigestValid(digest) || maximum == 0U || maximum > UMI_LEARNING_READER_MAX_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningPack *pack = NULL;
    UmiLearningReader *reader = NULL;
    UmiLearningPackInfo info;
    UmiLearningPackResource resource;
    UmiStatus status = UmiLearningShelfCopyPack(shelf, pack_id, &pack);
    if (status != UMI_STATUS_OK) return status;
    status = UmiLearningPackCopyInfo(pack, &info);
    if (status == UMI_STATUS_OK && info.revision != revision) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiLearningPackFindResource(pack, resource_id, &resource);
    if (status == UMI_STATUS_OK && strcmp(resource.sha256, digest) != 0)
        status = UMI_STATUS_INVALID_STATE;

    /* Ownership commits only after both metadata and content have been checked.
     * ReaderCreate consumes pack on success; destruction below handles either
     * side of that transfer without exposing a half-open reader to a frontend. */
    if (status == UMI_STATUS_OK) status = UmiLearningReaderCreate(&pack, maximum, &reader);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderOpen(reader, resource_id);
    UmiLearningPackDestroy(pack);
    if (status != UMI_STATUS_OK) {
        UmiLearningReaderDestroy(reader);
        return status;
    }
    *out = reader;
    return UMI_STATUS_OK;
}
