/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/release_gate.c
 *
 * PURPOSE:
 *   release-readiness gate combining parity, regression, accessibility and blocker evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/release_gate.h"

/*
 * Provide the fc release gate accepts operation used by this module and its client
 * applications.
 */
bool umi_fc_release_gate_accepts(const UmiFcReleaseGate *gate,double score,double accessibility,double regression,size_t blockers){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(gate==NULL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(gate->require_zero_blockers&&blockers>0U)return false;return score>=gate->minimum_score&&accessibility>=gate->minimum_accessibility&&regression<=gate->maximum_regression;}
