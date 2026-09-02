/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/validation_result.c
 *
 * PURPOSE:
 *   Aggregate gate outcomes into a deterministic validation decision and blocker count.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/validation_result.h"
#include <string.h>
/* Reset validation aggregation. */
void umi_helix_validation_result_init(UmiHelixValidationResult *r){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(r!=NULL) memset(r,0,sizeof(*r)); }
/* Count successful and failed gate evidence while preserving blocker semantics. */
void umi_helix_validation_result_record(UmiHelixValidationResult *r,bool passed,bool blocking){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL){ return; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(passed) r->passed++; /* Use this fallback path when the earlier condition does not apply. */ else {r->failed++; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(blocking) r->blocked++;} }
/* Treat missing blocking evidence as a blocker rather than an implicit pass. */
void umi_helix_validation_result_missing(UmiHelixValidationResult *r,bool blocking){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL){ return; } r->missing++; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(blocking) r->blocked++; }
/* Derive aggregate score and require zero blockers for automatic advancement. */
UmiHelixDecision umi_helix_validation_result_finalize(UmiHelixValidationResult *r){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL){ return UMI_HELIX_DECISION_DENY; } size_t total=r->passed+r->failed+r->missing; r->score=total==0U?0.0:(double)r->passed/(double)total; r->decision=r->blocked==0U&&r->failed==0U&&r->missing==0U?UMI_HELIX_DECISION_ALLOW:UMI_HELIX_DECISION_REVIEW; return r->decision; }
