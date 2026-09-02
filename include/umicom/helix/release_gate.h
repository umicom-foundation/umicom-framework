/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/release_gate.h
 *
 * PURPOSE:
 *   Evaluate mandatory Helix promotion gates before creating a new generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Promotion requires evidence, fitness, approval and rollback readiness instead of accepting a model's own claim that a candidate is safe.
 */

#ifndef INCLUDE_UMICOM_HELIX_RELEASE_GATE_H
#define INCLUDE_UMICOM_HELIX_RELEASE_GATE_H

#include <stdint.h>
#include "umicom/helix/fitness.h"
#include "umicom/helix/policy.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix release gate data shared with callers of this public contract.
 */
typedef struct UmiHelixReleaseGate {
    uint32_t required_checks;
    uint32_t passed_checks;
    int human_approved;
    int rollback_ready;
} UmiHelixReleaseGate;

/**
 * Provide the helix release gate check operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_release_gate_check(const UmiHelixReleaseGate *gate,
                                       const UmiHelixFitness *fitness,
                                       const UmiHelixPolicy *policy);

#ifdef __cplusplus
}
#endif

#endif
