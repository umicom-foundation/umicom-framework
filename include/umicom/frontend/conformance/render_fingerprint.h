/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/render_fingerprint.h
 *
 * PURPOSE:
 *   stable render fingerprints independent of toolkit-specific object addresses.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_RENDER_FINGERPRINT_H
#define UMICOM_FRONTEND_CONFORMANCE_RENDER_FINGERPRINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

uint64_t umi_fc_render_fingerprint_begin(const char *surface_id);
uint64_t umi_fc_render_fingerprint_add(uint64_t seed,const char *role,const char *semantic_id,int32_t order);

#ifdef __cplusplus
}
#endif
#endif
