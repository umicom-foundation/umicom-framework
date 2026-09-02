/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/quality_gate.c
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
#include "umicom/developer/helix/quality_gate.h"
#include <string.h>
/* Initialise conservative policy values so omitted configuration cannot widen autonomy. */
void umi_helix_quality_gate_init(UmiHelixQualityGate *g){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL){ return; } memset(g,0,sizeof(*g)); g->structure_size=(uint32_t)sizeof(*g); g->api_version=UMI_HELIX_API_VERSION; g->minimum_score=1.0; g->maximum_regression=0.0; }
/* Evaluate gate evidence deterministically. */
UmiHelixDecision umi_helix_quality_gate_evaluate(const UmiHelixQualityGate *g,double quality,double regression){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||g->structure_size!=sizeof(*g)||g->api_version!=UMI_HELIX_API_VERSION){ return UMI_HELIX_DECISION_DENY; } return quality>=g->minimum_score && regression<=g->maximum_regression?UMI_HELIX_DECISION_ALLOW:UMI_HELIX_DECISION_REVIEW; }
