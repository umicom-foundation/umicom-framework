/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/render_fingerprint.h
 *
 * PURPOSE:
 *   stable render fingerprints independent of toolkit-specific object addresses.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Provide the fc render fingerprint begin operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_render_fingerprint_begin(const char *surface_id);
/**
 * Add fc render fingerprint only after its inputs and available capacity have been
 * checked.
 */
uint64_t umi_fc_render_fingerprint_add(uint64_t seed,const char *role,const char *semantic_id,int32_t order);

#ifdef __cplusplus
}
#endif
#endif
