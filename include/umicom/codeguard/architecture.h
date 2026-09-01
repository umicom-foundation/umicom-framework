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
#include "umicom/codeguard/result.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_codeguard_architecture_scan_file(const char *root, const char *path,
                                               const UmiCodeGuardProfile *profile,
                                               UmiCodeGuardResult *result);
#ifdef __cplusplus
}
#endif
#endif
