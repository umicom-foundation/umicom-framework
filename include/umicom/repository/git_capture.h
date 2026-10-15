/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/git_capture.h
 * PURPOSE: Capture bounded Git protocol bytes through the shared native process runner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_REPOSITORY_GIT_CAPTURE_H
#define UMICOM_REPOSITORY_GIT_CAPTURE_H
#include "umicom/repository/inspection_types.h"
#include "umicom/platform/cancellation.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_REPOSITORY_GIT_CAPTURE_LIMIT (4U * 1024U * 1024U)
/** Owned raw output and independent process evidence. bytes may contain NULs;
 * length, not strlen, defines the data. One extra trailing NUL is convenience
 * only. A failed call may retain partial bytes, which are never a whole snapshot. */
typedef struct UmiRepositoryGitCaptureResult {
    unsigned char *bytes;
    size_t length;
    int exit_code;
    int launched;
    int timed_out;
    int cancelled;
    int termination_requested;
    int output_truncated;
    UmiStatus status;
} UmiRepositoryGitCaptureResult;
/**
 * Capture a selected Git argument vector as bytes, including embedded NULs.
 * maximumBytes must be 1..UMI_REPOSITORY_GIT_CAPTURE_LIMIT. Both streams are
 * merged; protocol consumers must reject diagnostics as malformed data.
 * Uses a hidden, owned process tree, the supplied directory and timeout.
 * No shell or implicit Git operation is added. Caller authorises the arguments.
 * Run on a worker; cancellation and all input pointers must outlive this call.
 * out must not own an earlier result: release it first. Failure clears output
 * before execution, then retains process and partial-output evidence if launched.
 */
UmiStatus UmiRepositoryGitCapture(const UmiRepositoryInspectionContext *context,
    const char *const *arguments, size_t argumentCount, size_t maximumBytes,
    const UmiCancellationToken *cancellation, UmiRepositoryGitCaptureResult *out);
/** Release owned bytes and clear the result. NULL and repeated release are safe.
 * Call only after capture returns; this is not a concurrent cancellation method. */
void UmiRepositoryGitCaptureRelease(UmiRepositoryGitCaptureResult *result);
#ifdef __cplusplus
}
#endif
#endif
