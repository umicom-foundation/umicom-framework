/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/renderer_profile.h
 *
 * PURPOSE:
 *   renderer identity, capability and policy metadata used by conformance evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_RENDERER_PROFILE_H
#define UMICOM_FRONTEND_CONFORMANCE_RENDERER_PROFILE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc renderer profile data shared with callers of this public contract.
 */
typedef struct UmiFcRendererProfile { char id[UMI_FC_ID_CAPACITY]; UmiFcFrontendKind kind; uint64_t capabilities; uint32_t api_version; bool production_ready; bool remote_session; } UmiFcRendererProfile;
/**
 * Provide the fc renderer profile make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_renderer_profile_make(const char *id,UmiFcFrontendKind kind,uint64_t capabilities,uint32_t api_version,UmiFcRendererProfile *out_profile);
/**
 * Check that fc renderer profile satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fc_renderer_profile_validate(const UmiFcRendererProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
