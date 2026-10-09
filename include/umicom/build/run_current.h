/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/run_current.h
 * PURPOSE: Prepare and queue one reviewed executable without invoking a build tool.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_RUN_CURRENT_H
#define UMICOM_BUILD_RUN_CURRENT_H
#include "umicom/build/project_session.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /** Resolve the selected program and working folder against the captured project
 * root. Bare program names are refused: select ./program, a project-relative
 * path containing a separator, or an absolute file. The file and directories
 * must exist. This does not prove executable format, source freshness or file
 * identity across replacement; the native launcher remains authoritative.
 * No process starts and failure leaves out unchanged. out may alias profile. */
    UmiStatus UmiBuildRunCurrentPrepare(const UmiBuildProfile *profile, UmiBuildProfile *out);
    /** Queue exactly one Run phase through the existing worker, cancellation, output
 * and optional job-history owners. No configure, build, test or save occurs.
 * The caller must pass the current workspace trust decision. A nonempty log
 * path requests a new exclusive output file; NULL disables capture.
 * Profile strings are copied before return. Submission success means queued,
 * not that the selected executable started or finished successfully. */
    UmiStatus UmiBuildProjectSessionRunCurrent(UmiBuildProjectSession *session,
                                               const UmiBuildProfile *profile, bool trusted,
                                               const char *log_path);
#ifdef __cplusplus
}
#endif
#endif
