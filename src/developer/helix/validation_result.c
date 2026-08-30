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
void umi_helix_validation_result_init(UmiHelixValidationResult *r){ if(r!=NULL) memset(r,0,sizeof(*r)); }
/* Count successful and failed gate evidence while preserving blocker semantics. */
void umi_helix_validation_result_record(UmiHelixValidationResult *r,bool passed,bool blocking){ if(r==NULL){ return; } if(passed) r->passed++; else {r->failed++; if(blocking) r->blocked++;} }
/* Treat missing blocking evidence as a blocker rather than an implicit pass. */
void umi_helix_validation_result_missing(UmiHelixValidationResult *r,bool blocking){ if(r==NULL){ return; } r->missing++; if(blocking) r->blocked++; }
/* Derive aggregate score and require zero blockers for automatic advancement. */
UmiHelixDecision umi_helix_validation_result_finalize(UmiHelixValidationResult *r){ if(r==NULL){ return UMI_HELIX_DECISION_DENY; } size_t total=r->passed+r->failed+r->missing; r->score=total==0U?0.0:(double)r->passed/(double)total; r->decision=r->blocked==0U&&r->failed==0U&&r->missing==0U?UMI_HELIX_DECISION_ALLOW:UMI_HELIX_DECISION_REVIEW; return r->decision; }
