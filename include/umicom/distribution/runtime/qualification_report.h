/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/qualification_report.h
 * Purpose: Export complete scoped evidence without raw logs or lossy test-name labels.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DISTRIBUTION_RUNTIME_QUALIFICATION_REPORT_H
#define UMICOM_DISTRIBUTION_RUNTIME_QUALIFICATION_REPORT_H
#include "umicom/distribution/runtime/qualification.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_QUALIFICATION_REPORT_LIMIT (160U * 1024U * 1024U)
/** Format an owned run as deterministic version-one TSV. Every selected target
 * and test appears; missing results are explicit and differ from NOT_RUN.
 * Names are hexadecimal UTF-8 bytes, so tabs/newlines cannot create fake rows.
 * Log contents are excluded; their supplied SHA-256 digests remain in evidence.
 * Reports may still contain private names. Save them to a user-chosen private
 * location, not automatically to a product repository or a profile directory.
 *
 * Call with NULL output and capacity zero to measure including the NUL byte.
 * Invalid arguments, exceeded limit or insufficient capacity preserve output
 * and outRequired. Inputs/output must not overlap. Serialize access to the run
 * for both measure and write; no files are opened and no job is started.
 * The output is an audit record, not an authenticated certificate or executable
 * replay format. Open attempts remain open; ready describes only this scope. */
UmiStatus UmiReleaseQualificationFormatReport(const UmiReleaseQualification *run,
    char *output, size_t capacity, size_t *outRequired);
const char *UmiReleaseQualificationOutcomeText(UmiReleaseQualificationOutcome outcome);
/** One individual blocker flag, not a mask. Unknown flags return "Unknown". */
const char *UmiReleaseQualificationBlockerText(UmiReleaseQualificationBlocker blocker);
#ifdef __cplusplus
}
#endif
#endif
