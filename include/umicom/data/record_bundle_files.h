/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Explicit, bounded file transfer for reviewed record history bundles. */
#ifndef UMICOM_DATA_RECORD_BUNDLE_FILES_H
#define UMICOM_DATA_RECORD_BUNDLE_FILES_H
#include "umicom/data/record_bundle.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Write a new plaintext file at an explicit absolute path. The trusted parent
 * must exist. Encoding completes before exclusive creation. Existing leaves,
 * including links, are never opened or removed. I/O failure may leave a partial
 * new file for diagnosis; caller chooses a new path after reviewing it. Parent
 * aliases may be followed by the output service, so choose a directory you own.
 * This never exports data automatically or creates directories. Run on a worker. */
UmiStatus umi_record_bundle_write_new(const char *path, const UmiRecordBundle *bundle);
/* Capture and export exactly the reviewed saved head, including its complete
 * bounded history. Canonical metadata must match before any output file is
 * created. A stale head is INVALID_STATE; unsaved revision zero is invalid.
 * Caller owns permission to disclose the record and chooses a trusted path.
 * Later database changes do not alter the captured file. The same exclusive
 * creation and partial-I/O limits as write_new apply. No draft is saved. */
UmiStatus umi_record_bundle_export_reviewed(UmiDataServer *server,
                                            const UmiRecordRevision *reviewed_head,
                                            const char *path);
/* Read a bounded regular file below an explicit absolute directory. Rooted-file
 * checks reject traversal, links and ambiguous names. File names are supplied by
 * the caller, never read from record metadata. Output remains unchanged on error.
 * Review and authorize the decoded contents before a separate restore call. */
UmiStatus umi_record_bundle_read(const char *directory, const char *relative_path,
                                 UmiRecordBundle **out);
#ifdef __cplusplus
}
#endif
#endif
