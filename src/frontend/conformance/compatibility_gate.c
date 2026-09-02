/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/compatibility_gate.c
 *
 * PURPOSE:
 *   application launch compatibility gate based on required semantic capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/compatibility_gate.h"

/*
 * Provide the fc compatibility gate evaluate operation used by this module and its client
 * applications.
 */
UmiFcOutcome umi_fc_compatibility_gate_evaluate(const UmiFcCompatibilityGate *gate,uint64_t actual,double score){uint64_t missing;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(gate==NULL)return UMI_FC_FAIL;missing=gate->required&~actual;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(missing==0U&&score>=gate->minimum_score)return UMI_FC_PASS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(gate->allow_degraded&&score>=gate->minimum_score&&missing!=gate->required)return UMI_FC_DEGRADED;return UMI_FC_FAIL;}
