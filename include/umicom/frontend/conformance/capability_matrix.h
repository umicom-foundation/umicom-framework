/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/capability_matrix.h
 *
 * PURPOSE:
 *   cross-frontend capability matrix used to compare GTK4, Qt6, Native Web and headless renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_CAPABILITY_MATRIX_H
#define UMICOM_FRONTEND_CONFORMANCE_CAPABILITY_MATRIX_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc capability matrix data shared with callers of this public contract.
 */
typedef struct UmiFcCapabilityMatrix { uint64_t masks[5]; } UmiFcCapabilityMatrix;
/**
 * Initialise fc capability matrix from caller-provided values so later operations receive
 * a known state.
 */
void umi_fc_capability_matrix_init(UmiFcCapabilityMatrix *matrix);
/**
 * Copy fc capability matrix into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_fc_capability_matrix_set(UmiFcCapabilityMatrix *matrix, UmiFcFrontendKind frontend, uint64_t mask);
/**
 * Provide the fc capability matrix get operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_capability_matrix_get(const UmiFcCapabilityMatrix *matrix, UmiFcFrontendKind frontend);
/**
 * Provide the fc capability matrix common operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_capability_matrix_common(const UmiFcCapabilityMatrix *matrix, const UmiFcFrontendKind *frontends, size_t count);

#ifdef __cplusplus
}
#endif
#endif
