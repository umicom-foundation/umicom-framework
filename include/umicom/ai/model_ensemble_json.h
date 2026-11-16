/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/model_ensemble_json.h
 *
 * PURPOSE:
 *   Export bounded model-comparison metadata while preserving exact revisions and omitting generated content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_MODEL_ENSEMBLE_JSON_H
#define UMICOM_AI_MODEL_ENSEMBLE_JSON_H
#include "umicom/ai/model_ensemble.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY (128U * 1024U)
/* Serialize an admitted, stable report into caller-owned UTF-8 JSON.
 * Schema 1 records request ID, exact revision as a decimal string, active and
 * successful counts, selected index or null, and source-ordered result rows.
 * Each row separates configured target identity from successful response
 * identity. Failed response storage is never read and becomes response:null.
 * Empty successful identity fields remain empty, not inferred from the target.
 * Status is its numeric value; unknown provider status codes are retained.
 *
 * Prompts, answer text, token usage, prices, latency and credentials are omitted.
 * The current report has no usage-known bits; default zero cannot establish a
 * measured token count. This receipt does not authenticate identity, approve a
 * request, certify an answer, or change selection/revision. No I/O occurs.
 *
 * Reuse shared structural validation, then validate emitted UTF-8 through the
 * JSON writer. Invalid arguments/records/text give INVALID_ARGUMENT; zero or
 * insufficient capacity gives CAPACITY_EXCEEDED. A private bounded staging
 * buffer is cleared before release. Output and out_length remain unchanged on
 * every failure. Capacity includes the NUL; returned length excludes it.
 * At most UMI_AI_MODEL_ENSEMBLE_JSON_CAPACITY bytes are allocated.
 * Caller report/output/length storage must be complete, stable, serialized and
 * non-overlapping. Copied JSON survives later report mutation or destruction. */
UmiStatus UmiAiModelEnsembleReportJson(const UmiAiModelEnsembleReport *report,
    char *out, size_t capacity, size_t *out_length);
#ifdef __cplusplus
}
#endif
#endif
