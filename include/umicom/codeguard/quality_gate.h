/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/quality_gate.h
 *
 * PURPOSE:
 *   Turn CodeGuard findings into a deterministic pass/fail release gate.
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
#ifndef UMICOM_CODEGUARD_QUALITY_GATE_H
#define UMICOM_CODEGUARD_QUALITY_GATE_H
#include "umicom/codeguard/result.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the code guard quality gate data shared with callers of this public contract.
 */
typedef struct UmiCodeGuardQualityGate {
    size_t max_critical;
    size_t max_high;
    size_t max_medium;
    size_t max_architecture;
} UmiCodeGuardQualityGate;
/**
 * Provide the codeguard quality gate default operation used by this module and its client
 * applications.
 */
UmiCodeGuardQualityGate umi_codeguard_quality_gate_default(void);
/**
 * Provide the codeguard quality gate pass operation used by this module and its client
 * applications.
 */
int umi_codeguard_quality_gate_pass(const UmiCodeGuardQualityGate *gate,
                                    const UmiCodeGuardResult *result);
#ifdef __cplusplus
}
#endif
#endif
