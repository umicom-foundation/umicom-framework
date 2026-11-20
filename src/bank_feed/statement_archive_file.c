/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/statement_archive_file.c
 *
 * PURPOSE:
 *   Exchange statement reviews as bounded local files and compare source digests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/bank_feed/statement_archive.h"
#include "umicom/base/sha256.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/input_stream.h"
#include "umicom/platform/output_file.h"
#include <string.h>

/* Keep filesystem admission at the shared boundary so a command, job or
 * desktop adapter reopens reviews with the same amount and schema rules. */
UmiStatus UmiBankStatementArchiveOpenFile(const char *path,
    UmiBankStatementArchive **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    unsigned char *bytes = NULL;
    size_t length = 0U;
    UmiStatus status = UmiInputFileRead(path, UMI_BANK_STATEMENT_ARCHIVE_MAX_BYTES,
        &bytes, &length);
    if (status == UMI_STATUS_OK) status = UmiBankStatementArchiveDecode(bytes, length, out);
    UmiInputFileFree(bytes);
    return status;
}

UmiStatus UmiBankStatementArchiveSaveNewFile(const UmiBankStatementArchive *archive,
    const char *path)
{
    UmiStatus status = UmiOutputFileValidatePath(path);
    char *bytes = NULL;
    size_t length = 0U;
    UmiOutputFile *file = NULL;
    if (status == UMI_STATUS_OK) status = UmiBankStatementArchiveEncode(archive, &bytes, &length);
    if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(path, &file);
    if (file != NULL) {
        status = UmiOutputFileWrite(file, bytes, length);
        const UmiStatus closed = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK) status = closed;
    }
    UmiOutputFileDestroy(file);
    UmiBankFeedArchiveFree(bytes);
    return status;
}

static UmiStatus HashSourceChunk(const unsigned char *bytes, size_t length, void *context)
{
    return UmiSha256Update(context, bytes, length);
}

UmiStatus UmiBankStatementArchiveCheckSourceFile(const UmiBankStatementArchive *archive,
    const char *path, UmiBankStatementSourceCheck *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiBankStatementArchiveSnapshot snapshot;
    UmiStatus status = UmiBankStatementArchiveRead(archive, &snapshot);
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.source.sourceSha256[0] == '\0') {
        *out = UMI_BANK_STATEMENT_SOURCE_DIGEST_UNRECORDED;
        return UMI_STATUS_OK;
    }

    /* Publish a comparison only when the shared reader has checked the whole
     * file and closed it. A prefix could also match a stale source claim. */
    UmiSha256 hash;
    UmiSha256Init(&hash);
    uint64_t length = 0U;
    status = UmiInputFileVisit(path, UMI_BANK_FEED_MAX_BYTES, HashSourceChunk, &hash, &length);
    unsigned char digest[32];
    if (status == UMI_STATUS_OK) status = UmiSha256Final(&hash, digest);
    if (status == UMI_STATUS_OK) {
        char hex[UMI_SHA256_HEX_CAPACITY];
        UmiSha256Hex(digest, hex);
        *out = strcmp(hex, snapshot.source.sourceSha256) == 0
            ? UMI_BANK_STATEMENT_SOURCE_DIGEST_MATCH : UMI_BANK_STATEMENT_SOURCE_DIGEST_MISMATCH;
    }
    return status;
}
