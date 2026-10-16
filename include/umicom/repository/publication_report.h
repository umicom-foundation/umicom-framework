/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/publication_report.h
 * PURPOSE: Export bounded publication results without collecting credentials or source contents.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_PUBLICATION_REPORT_H
#define UMICOM_REPOSITORY_PUBLICATION_REPORT_H
#include "umicom/repository/publication.h"
#include "umicom/platform/output_file.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Identify what was attempted, independently of the returned Git status. */
typedef enum UmiRepositoryPublicationReportMode {
    UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW = 0,
    UMI_REPOSITORY_PUBLICATION_REPORT_CACHED = 1,
    UMI_REPOSITORY_PUBLICATION_REPORT_APPLY = 2
} UmiRepositoryPublicationReportMode;

/** Format one immutable snapshot as UTF-8 JSON, including a final newline.
 * required_capacity includes the terminating NUL. Pass NULL/0 for a size query.
 * Invalid input or insufficient capacity leaves the destination unchanged.
 * Unterminated fields, invalid UTF-8, counts outside the bounded entry array and
 * invalid entry references are rejected before serialization. On invalid input
 * required_capacity is zero. Keep report, output and required_capacity separate
 * and serialize access; this function does not take a concurrent snapshot.
 * The schema includes local paths and action flags, but no remote URL, commit
 * message, file content or raw Git output. Store it outside public repositories.
 * Updates count actions across graph refreshes; entries describe the latest
 * discovered graph. A successful preview does not mean anything was published. */
UmiStatus umi_repository_publication_report_format_json(
    const UmiRepositoryPublicationReport *report,
    UmiRepositoryPublicationReportMode mode, char *output, size_t capacity,
    size_t *required_capacity);

/** Write a complete formatted snapshot to a caller-owned open output file.
 * The caller reserves a new file with UmiOutputFileCreate before publication,
 * then closes and destroys it after this call. A write failure retains partial
 * evidence. This operation never retries Git, overwrites an existing file or
 * promises recovery after process termination or power loss. */
UmiStatus umi_repository_publication_report_write_json(
    const UmiRepositoryPublicationReport *report,
    UmiRepositoryPublicationReportMode mode, UmiOutputFile *file);

/** Check an absolute evidence path before exclusive creation.
 * Reject a path inside a discovered checkout or beneath an existing .git marker,
 * including repositories outside the selected workspace. The parent directory
 * must exist. This is a placement check, not a filesystem sandbox: use trusted
 * directories, without aliases or concurrent directory replacement. No files
 * are created, modified or removed. A failed publication report is acceptable. */
UmiStatus umi_repository_publication_report_check_path(
    const UmiRepositoryPublicationReport *report, const char *path);

#ifdef __cplusplus
}
#endif
#endif
