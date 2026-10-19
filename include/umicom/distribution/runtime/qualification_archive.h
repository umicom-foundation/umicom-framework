/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/qualification_archive.h
 * Purpose: Save and reopen identified qualification evidence in an owner-selected private location.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DISTRIBUTION_QUALIFICATION_ARCHIVE_H
#define UMICOM_DISTRIBUTION_QUALIFICATION_ARCHIVE_H
#include "umicom/distribution/runtime/qualification_view.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiQualificationArchiveReceipt {
    UmiReleaseQualificationIdentity identity;
    char reportSha256[65];
    size_t bytes;
} UmiQualificationArchiveReceipt;
/** Parse the canonical version-one report into an owned immutable saved view.
 * Validate framing, bounds, exact identities, sorted unique names, all result
 * invariants, derived counters/blockers and the recomputed plan fingerprint.
 * Report bytes may describe an open or closed attempt; they never resume a run.
 * Reports are untrusted historical claims, not signatures or proof that current
 * files match. A consistent "passed" record must still be reviewed with its logs.
 * Maximum bytes: UMI_QUALIFICATION_REPORT_LIMIT, declared in qualification_report.h.
 * *outView must be NULL; all outputs remain unchanged on refusal. No I/O. */
UmiStatus UmiQualificationArchiveParse(const char *bytes, size_t length,
    UmiQualificationView **outView, UmiQualificationArchiveReceipt *outReceipt);
/** Save a closed attempt using exclusive creation at an explicit absolute path.
 * The existing parent must be trusted and owner-selected. No profile-directory
 * fallback, overwrite, directory creation, cleanup, Git or publication occurs.
 * Serialize access to run through return. Failed/partial files remain available
 * for investigation and will fail strict loading; choose a new name to retry.
 * Receipt is assigned only after write and close succeed. Disk I/O may block. */
UmiStatus UmiQualificationArchiveSave(const UmiReleaseQualification *run, const char *absolutePath,
    UmiQualificationArchiveReceipt *outReceipt);
/** Read one bounded regular file, optionally requiring an externally retained
 * lowercase SHA-256 before parsing. NULL expectedSha256 means no external match.
 * A digest detects changed bytes only against a separately trusted receipt;
 * it does not authenticate an author or confirm test execution. Final symlinks
 * and devices are refused by the shared native reader; parent aliases may be
 * followed. Loading never executes commands or reads paths named in the report.
 * *outView must be NULL; outputs remain unchanged on failure. */
UmiStatus UmiQualificationArchiveLoad(const char *absolutePath, const char *expectedSha256,
    UmiQualificationView **outView, UmiQualificationArchiveReceipt *outReceipt);
#ifdef __cplusplus
}
#endif
#endif
