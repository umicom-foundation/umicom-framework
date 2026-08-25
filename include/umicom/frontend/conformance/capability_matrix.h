/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/capability_matrix.h
 *
 * PURPOSE:
 *   cross-frontend capability matrix used to compare GTK4, Qt6, Native Web and headless renderers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcCapabilityMatrix { uint64_t masks[5]; } UmiFcCapabilityMatrix;
void umi_fc_capability_matrix_init(UmiFcCapabilityMatrix *matrix);
UmiStatus umi_fc_capability_matrix_set(UmiFcCapabilityMatrix *matrix, UmiFcFrontendKind frontend, uint64_t mask);
uint64_t umi_fc_capability_matrix_get(const UmiFcCapabilityMatrix *matrix, UmiFcFrontendKind frontend);
uint64_t umi_fc_capability_matrix_common(const UmiFcCapabilityMatrix *matrix, const UmiFcFrontendKind *frontends, size_t count);

#ifdef __cplusplus
}
#endif
#endif
