/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/quality_gate.c
 *
 * PURPOSE:
 *   Apply deterministic severity and architecture thresholds to a CodeGuard
 *   result.
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

#include "umicom/codeguard/quality_gate.h"
UmiCodeGuardQualityGate umi_codeguard_quality_gate_default(void){UmiCodeGuardQualityGate g={0U,0U,1000000U,0U};return g;}
int umi_codeguard_quality_gate_pass(const UmiCodeGuardQualityGate *g,const UmiCodeGuardResult *r){if(g==NULL||r==NULL)return 0;return umi_codeguard_result_count_severity(r,UMI_CODEGUARD_CRITICAL)<=g->max_critical&&umi_codeguard_result_count_severity(r,UMI_CODEGUARD_HIGH)<=g->max_high&&umi_codeguard_result_count_severity(r,UMI_CODEGUARD_MEDIUM)<=g->max_medium&&umi_codeguard_result_count_category(r,UMI_CODEGUARD_CATEGORY_ARCHITECTURE)<=g->max_architecture;}
