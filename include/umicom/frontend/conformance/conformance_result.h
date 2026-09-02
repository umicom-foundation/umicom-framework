/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/conformance_result.h
 *
 * PURPOSE:
 *   normalised pass, degraded and fail outcomes for one conformance case.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_CONFORMANCE_RESULT_H
#define UMICOM_FRONTEND_CONFORMANCE_CONFORMANCE_RESULT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"
#include "umicom/frontend/conformance/conformance_case.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc conformance result data shared with callers of this public contract.
 */
typedef struct UmiFcConformanceResult { char case_id[UMI_FC_ID_CAPACITY]; UmiFcOutcome outcome; uint64_t missing; double score; } UmiFcConformanceResult;
/**
 * Provide the fc conformance result from case operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_conformance_result_from_case(const UmiFcConformanceCase *item,UmiFcConformanceResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
