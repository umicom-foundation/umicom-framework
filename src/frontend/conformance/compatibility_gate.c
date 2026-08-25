/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/compatibility_gate.c
 *
 * PURPOSE:
 *   application launch compatibility gate based on required semantic capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/compatibility_gate.h"

UmiFcOutcome umi_fc_compatibility_gate_evaluate(const UmiFcCompatibilityGate *gate,uint64_t actual,double score){uint64_t missing;if(gate==NULL)return UMI_FC_FAIL;missing=gate->required&~actual;if(missing==0U&&score>=gate->minimum_score)return UMI_FC_PASS;if(gate->allow_degraded&&score>=gate->minimum_score&&missing!=gate->required)return UMI_FC_DEGRADED;return UMI_FC_FAIL;}
