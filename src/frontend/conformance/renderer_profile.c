/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/renderer_profile.c
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
#include "umicom/frontend/conformance/renderer_profile.h"

/*
 * Provide the fc renderer profile make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_renderer_profile_make(const char *id,UmiFcFrontendKind kind,uint64_t capabilities,uint32_t api_version,UmiFcRendererProfile *out_profile){ UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_profile==NULL)return UMI_STATUS_INVALID_ARGUMENT; *out_profile=(UmiFcRendererProfile){0}; st=umi_fc_copy_text(out_profile->id,sizeof(out_profile->id),id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; out_profile->kind=kind; out_profile->capabilities=capabilities; out_profile->api_version=api_version; return umi_fc_renderer_profile_validate(out_profile); }
/*
 * Check that fc renderer profile satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fc_renderer_profile_validate(const UmiFcRendererProfile *profile){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile==NULL||profile->id[0]=='\0'||profile->api_version==0U||profile->kind<UMI_FC_FRONTEND_GTK4||profile->kind>UMI_FC_FRONTEND_HEADLESS)return UMI_STATUS_INVALID_ARGUMENT; return UMI_STATUS_OK; }
