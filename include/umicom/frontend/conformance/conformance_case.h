/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/conformance_case.h
 *
 * PURPOSE:
 *   deterministic conformance-case inputs used by automated frontend tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_CONFORMANCE_CASE_H
#define UMICOM_FRONTEND_CONFORMANCE_CONFORMANCE_CASE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc conformance case data shared with callers of this public contract.
 */
typedef struct UmiFcConformanceCase { char id[UMI_FC_ID_CAPACITY]; uint64_t required; uint64_t actual; bool degradation_allowed; } UmiFcConformanceCase;
/**
 * Provide the fc conformance case make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_conformance_case_make(const char *id,uint64_t required,uint64_t actual,bool degradation_allowed,UmiFcConformanceCase *out_case);
/**
 * Provide the fc conformance case missing operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_conformance_case_missing(const UmiFcConformanceCase *item);

#ifdef __cplusplus
}
#endif
#endif
