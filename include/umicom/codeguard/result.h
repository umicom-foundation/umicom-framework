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

/*
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
/**
 * Represent the code guard result data shared with callers of this public contract.
 */
typedef struct UmiCodeGuardResult UmiCodeGuardResult;
/**
 * Initialise codeguard result from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_codeguard_result_create(size_t initial_capacity, UmiCodeGuardResult **out_result);
/**
 * Release or reset state held by codeguard result so the same storage can be reused
 * safely.
 */
void umi_codeguard_result_destroy(UmiCodeGuardResult *result);
/**
 * Release or reset state held by codeguard result so the same storage can be reused
 * safely.
 */
void umi_codeguard_result_clear(UmiCodeGuardResult *result);
/**
 * Add codeguard result only after its inputs and available capacity have been checked.
 */
UmiStatus umi_codeguard_result_add(UmiCodeGuardResult *result, const UmiCodeGuardFinding *finding);
/**
 * Return the number of records represented by codeguard result without changing their
 * state.
 */
size_t umi_codeguard_result_count(const UmiCodeGuardResult *result);
/**
 * Find codeguard result while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCodeGuardFinding *umi_codeguard_result_at(const UmiCodeGuardResult *result, size_t index);
/**
 * Provide the codeguard result count severity operation used by this module and its client
 * applications.
 */
size_t umi_codeguard_result_count_severity(const UmiCodeGuardResult *result, UmiCodeGuardSeverity severity);
/**
 * Provide the codeguard result count category operation used by this module and its client
 * applications.
 */
size_t umi_codeguard_result_count_category(const UmiCodeGuardResult *result, UmiCodeGuardCategory category);
#ifdef __cplusplus
}
#endif
#endif
