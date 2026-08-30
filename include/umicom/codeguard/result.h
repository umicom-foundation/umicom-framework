/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/result.h
 *
 * PURPOSE:
 *   Own the growable list of CodeGuard findings produced by a scan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_RESULT_H
#define UMICOM_CODEGUARD_RESULT_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/codeguard/finding.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCodeGuardResult UmiCodeGuardResult;
UmiStatus umi_codeguard_result_create(size_t initial_capacity, UmiCodeGuardResult **out_result);
void umi_codeguard_result_destroy(UmiCodeGuardResult *result);
void umi_codeguard_result_clear(UmiCodeGuardResult *result);
UmiStatus umi_codeguard_result_add(UmiCodeGuardResult *result, const UmiCodeGuardFinding *finding);
size_t umi_codeguard_result_count(const UmiCodeGuardResult *result);
const UmiCodeGuardFinding *umi_codeguard_result_at(const UmiCodeGuardResult *result, size_t index);
size_t umi_codeguard_result_count_severity(const UmiCodeGuardResult *result, UmiCodeGuardSeverity severity);
size_t umi_codeguard_result_count_category(const UmiCodeGuardResult *result, UmiCodeGuardCategory category);
#ifdef __cplusplus
}
#endif
#endif
