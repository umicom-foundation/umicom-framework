/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/record_draft_files.h
 *
 * PURPOSE:
 *   Explicit private recovery files reuse Framework file ownership contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Explicit private recovery files reuse Framework file ownership contracts. */
#ifndef UMICOM_DATA_RECORD_DRAFT_FILES_H
#define UMICOM_DATA_RECORD_DRAFT_FILES_H
#include "umicom/data/record_draft_snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Write one NEW plaintext recovery file at an absolute path. Its trusted parent
 * must exist. Existing files are never opened, replaced or removed. Encoding
 * finishes before creation; an I/O failure may leave a partial new file for
 * diagnosis. No background save, pruning or directories are created here.
 * Run storage calls on a worker with an immutable captured snapshot. */
UmiStatus umi_record_draft_snapshot_write_new(const char *path, const UmiRecordDraftSnapshot *snapshot);
/* Read a bounded ordinary file beneath an explicit absolute directory using
 * Framework rooted-file checks. The filename and parent come from the caller,
 * never the encoded metadata. Malformed or partial files leave output unchanged.
 * Successful decoding is neither authorization nor proof of a matching store;
 * use check_store before restoring against a selected database. */
UmiStatus umi_record_draft_snapshot_read(const char *directory, const char *relative_path, UmiRecordDraftSnapshot *output);
#ifdef __cplusplus
}
#endif
#endif
