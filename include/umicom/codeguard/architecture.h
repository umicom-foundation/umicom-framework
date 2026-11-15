/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/architecture.h
 *
 * PURPOSE:
 *   Enforce Umicom dependency boundaries and flag tight-coupling indicators.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_ARCHITECTURE_H
#define UMICOM_CODEGUARD_ARCHITECTURE_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/codeguard/profile.h"
#include "umicom/platform/cancellation.h"
#include "umicom/codeguard/result.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the codeguard architecture scan file operation used by this module and its
 * client applications.
 */
/* Append findings on success; on failure, appended findings can be partial.
 * Callers must check the status, including measurement, insertion and stream
 * failures. Use the retained CodeGuard service to publish complete reports.
 * Rule matching remains a textual heuristic with the existing path policy. */
UmiStatus umi_codeguard_architecture_scan_file(const char *root, const char *path,
                                               const UmiCodeGuardProfile *profile,
                                               UmiCodeGuardResult *result);

/* Both entry points enforce profile.max_file_bytes on measurement and every
 * policy read. The cancellable variant borrows the token for the entire call.
 * Findings appended before a later failure are not a complete report; callers
 * needing rollback should use the retained scan service's candidate ownership. */
UmiStatus umi_codeguard_architecture_scan_file_cancellable(const char *root,
    const char *path, const UmiCodeGuardProfile *profile, UmiCodeGuardResult *result,
    const UmiCancellationToken *cancellation);
#ifdef __cplusplus
}
#endif
#endif
