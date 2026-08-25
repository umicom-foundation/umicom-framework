/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/validation_result.h
 *
 * PURPOSE:
 *   Aggregate gate outcomes into a deterministic validation decision and blocker count.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_VALIDATION_RESULT_H
#define UMICOM_DEVELOPER_HELIX_VALIDATION_RESULT_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixValidationResult { size_t passed; size_t failed; size_t blocked; size_t missing; double score; UmiHelixDecision decision; } UmiHelixValidationResult;
/* Initialise aggregate validation counters. */
void umi_helix_validation_result_init(UmiHelixValidationResult *result);
/* Record a completed validation gate. */
void umi_helix_validation_result_record(UmiHelixValidationResult *result,bool passed,bool blocking);
/* Record unavailable required evidence. */
void umi_helix_validation_result_missing(UmiHelixValidationResult *result,bool blocking);
/* Finalise score and conservative allow/review decision. */
UmiHelixDecision umi_helix_validation_result_finalize(UmiHelixValidationResult *result);
#ifdef __cplusplus
}
#endif
#endif
