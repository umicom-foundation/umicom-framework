/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/safety_gate.h
 *
 * PURPOSE:
 *   Block changes that exceed scope, destructive-operation or risk constraints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_SAFETY_GATE_H
#define UMICOM_DEVELOPER_HELIX_SAFETY_GATE_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixSafetyGate { uint32_t structure_size; uint32_t api_version; double maximum_risk; bool allow_destructive; } UmiHelixSafetyGate;
/* Initialise conservative gate defaults. */
void umi_helix_safety_gate_init(UmiHelixSafetyGate *gate);
/* Evaluate supplied evidence without performing repository mutation. */
UmiHelixDecision umi_helix_safety_gate_evaluate(const UmiHelixSafetyGate *gate,double risk,bool destructive);
#ifdef __cplusplus
}
#endif
#endif
