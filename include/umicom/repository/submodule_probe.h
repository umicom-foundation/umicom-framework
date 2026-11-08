/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/submodule_probe.h
 *
 * PURPOSE:
 *   Populate existing submodule maintenance counts from recursive submodule status.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_REPOSITORY_SUBMODULE_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_SUBMODULE_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"
#include "umicom/repository/submodule_status.h"
#include "umicom/repository/git_capture.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Read recursive submodule status and preserve the existing maintenance summary type. */
UmiStatus umi_repository_submodule_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositorySubmoduleStatus *out_value);

/** Capture the read-only recursive Git status operation through Framework's
 * owned process runner. No init, update, fetch, stage or pin change is requested.
 * The caller releases out with UmiRepositoryGitCaptureRelease. Partial bytes and
 * process failure flags remain evidence, never a complete status observation.
 * Both streams are merged by the existing capture service. */
UmiStatus UmiRepositorySubmoduleCapture(const UmiRepositoryInspectionContext *context,
    const UmiCancellationToken *cancellation, UmiRepositoryGitCaptureResult *out);

/** Summarise bounded status lines only after the complete input is valid.
 * Empty input represents no reported submodules. Nonempty input must end in LF;
 * CRLF is accepted. Each line requires a known marker, a 40/64-digit object name
 * and a nonempty path/display tail without control bytes. The tail is not parsed
 * as a machine path. Diagnostics, NUL and incomplete lines are rejected.
 * Output is unchanged on failure. dirty counts U (merge-conflicted) entries;
 * these counts do not detect every modified file inside child working trees. */
UmiStatus UmiRepositorySubmoduleStatusParse(const unsigned char *bytes, size_t length,
    UmiRepositorySubmoduleStatus *out);
#ifdef __cplusplus
}
#endif
#endif
