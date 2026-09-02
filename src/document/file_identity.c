/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/file_identity.c
 *
 * PURPOSE:
 *   Implement portable file metadata and content-based external-change checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/file_identity.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#define UMI_ACCESS _access
#else
#include <unistd.h>
#define UMI_ACCESS access
#endif

#include "umicom/document/fingerprint.h"

/*
 * Provide the document file info operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_file_info(const char *path,
                                 UmiDocumentFileInfo *out_info)
{
    struct stat information;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_info == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_info, 0, sizeof(*out_info));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(path) >= sizeof(out_info->path)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(out_info->path, sizeof(out_info->path), "%s", path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (stat(path, &information) != 0) return UMI_STATUS_NOT_FOUND;
    out_info->exists = 1;
    out_info->regular_file = S_ISREG(information.st_mode);
    out_info->byte_count = (uint64_t)information.st_size;
    out_info->modified_time_seconds = (int64_t)information.st_mtime;
    out_info->readable = UMI_ACCESS(path, 4) == 0;
    out_info->writable = UMI_ACCESS(path, 2) == 0;
    return UMI_STATUS_OK;
}

/*
 * Provide the document file changed operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_file_changed(const char *path,
                                    const UmiDocumentFingerprint *baseline,
                                    int *out_changed,
                                    UmiDocumentFingerprint *out_current)
{
    UmiDocumentFingerprint current;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || baseline == NULL || out_changed == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_document_fingerprint_file(path, &current);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_IO_ERROR) {
        *out_changed = baseline->valid;
        return status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    *out_changed = !umi_document_fingerprint_equal(baseline, &current);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_current != NULL) *out_current = current;
    return UMI_STATUS_OK;
}
