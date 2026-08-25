/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/release_gate.c
 *
 * PURPOSE:
 *   release-readiness gate combining parity, regression, accessibility and blocker evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/release_gate.h"

bool umi_fc_release_gate_accepts(const UmiFcReleaseGate *gate,double score,double accessibility,double regression,size_t blockers){if(gate==NULL)return false;if(gate->require_zero_blockers&&blockers>0U)return false;return score>=gate->minimum_score&&accessibility>=gate->minimum_accessibility&&regression<=gate->maximum_regression;}
