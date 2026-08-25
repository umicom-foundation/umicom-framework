/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/safety_gate.c
 *
 * PURPOSE:
 *   Block changes that exceed scope, destructive-operation or risk constraints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/safety_gate.h"
#include <string.h>
/* Initialise conservative policy values so omitted configuration cannot widen autonomy. */
void umi_helix_safety_gate_init(UmiHelixSafetyGate *g){ if(g==NULL){ return; } memset(g,0,sizeof(*g)); g->structure_size=(uint32_t)sizeof(*g); g->api_version=UMI_HELIX_API_VERSION; g->maximum_risk=0.0; g->allow_destructive=false; }
/* Evaluate gate evidence deterministically. */
UmiHelixDecision umi_helix_safety_gate_evaluate(const UmiHelixSafetyGate *g,double risk,bool destructive){ if(g==NULL||g->structure_size!=sizeof(*g)||g->api_version!=UMI_HELIX_API_VERSION){ return UMI_HELIX_DECISION_DENY; } if(risk>g->maximum_risk){ return UMI_HELIX_DECISION_DENY; } if(destructive&&!g->allow_destructive){ return UMI_HELIX_DECISION_REVIEW; } return UMI_HELIX_DECISION_ALLOW; }
