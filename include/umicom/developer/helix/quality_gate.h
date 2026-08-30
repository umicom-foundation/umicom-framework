/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/quality_gate.h
 *
 * PURPOSE:
 *   Apply quality-score and regression thresholds before an engineering result may advance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_QUALITY_GATE_H
#define UMICOM_DEVELOPER_HELIX_QUALITY_GATE_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixQualityGate { uint32_t structure_size; uint32_t api_version; double minimum_score; double maximum_regression; } UmiHelixQualityGate;
/* Initialise conservative gate defaults. */
void umi_helix_quality_gate_init(UmiHelixQualityGate *gate);
/* Evaluate supplied evidence without performing repository mutation. */
UmiHelixDecision umi_helix_quality_gate_evaluate(const UmiHelixQualityGate *gate,double quality,double regression);
#ifdef __cplusplus
}
#endif
#endif
