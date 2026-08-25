/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/renderer_profile.h
 *
 * PURPOSE:
 *   renderer identity, capability and policy metadata used by conformance evaluation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcRendererProfile { char id[UMI_FC_ID_CAPACITY]; UmiFcFrontendKind kind; uint64_t capabilities; uint32_t api_version; bool production_ready; bool remote_session; } UmiFcRendererProfile;
UmiStatus umi_fc_renderer_profile_make(const char *id,UmiFcFrontendKind kind,uint64_t capabilities,uint32_t api_version,UmiFcRendererProfile *out_profile);
UmiStatus umi_fc_renderer_profile_validate(const UmiFcRendererProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
