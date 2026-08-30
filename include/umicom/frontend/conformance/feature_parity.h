/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/feature_parity.h
 *
 * PURPOSE:
 *   missing, extra and shared capability calculation between canonical and candidate renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_FEATURE_PARITY_H
#define UMICOM_FRONTEND_CONFORMANCE_FEATURE_PARITY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcFeatureParity { uint64_t shared; uint64_t missing; uint64_t extra; double score; } UmiFcFeatureParity;
UmiStatus umi_fc_feature_parity_calculate(uint64_t canonical,uint64_t candidate,UmiFcFeatureParity *out_parity);

#ifdef __cplusplus
}
#endif
#endif
