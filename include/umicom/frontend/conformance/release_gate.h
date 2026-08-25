/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/release_gate.h
 *
 * PURPOSE:
 *   release-readiness gate combining parity, regression, accessibility and blocker evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_RELEASE_GATE_H
#define UMICOM_FRONTEND_CONFORMANCE_RELEASE_GATE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcReleaseGate { double minimum_score; double minimum_accessibility; double maximum_regression; bool require_zero_blockers; } UmiFcReleaseGate;
bool umi_fc_release_gate_accepts(const UmiFcReleaseGate *gate,double score,double accessibility,double regression,size_t blockers);

#ifdef __cplusplus
}
#endif
#endif
