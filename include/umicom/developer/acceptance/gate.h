/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/acceptance/gate.h
 *
 * PURPOSE:
 *   Evaluate capability gates against explicit Framework acceptance evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_ACCEPTANCE_GATE_H
#define UMICOM_DEVELOPER_ACCEPTANCE_GATE_H

#include "umicom/developer/acceptance/evidence.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Evaluate every required capability id against the supplied evidence store.
 * Missing, failed and blocked requirements are reported separately so Studio
 * and release tooling can show the actual reason a gate is not complete.
 */
UmiStatus umi_developer_acceptance_gate_evaluate(
    const UmiDeveloperAcceptanceGate *gate,
    const UmiDeveloperAcceptanceEvidenceStore *evidence,
    UmiDeveloperAcceptanceGateResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
