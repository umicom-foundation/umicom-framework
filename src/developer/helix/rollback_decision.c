/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/rollback_decision.c
 *
 * PURPOSE:
 *   Evaluate validation, safety, build and test evidence to choose continue, stop or rollback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/rollback_decision.h"
/* Keep destructive rollback decisions deterministic and evidence-driven. */
UmiHelixDecision umi_helix_rollback_decision_evaluate(bool safety,bool build,bool tests,double regression,double max_regression){ if(safety||build||tests){ return UMI_HELIX_DECISION_ROLLBACK; } if(regression>max_regression){ return UMI_HELIX_DECISION_REVIEW; } return UMI_HELIX_DECISION_ALLOW; }
