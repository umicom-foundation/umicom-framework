/* Umicom Framework | Compiler capability snapshots | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/capability.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_compiler_capability_capture(const UmiCompilerProvider *provider,const UmiCompilerProfile *profile,UmiCompilerCapabilitySnapshot *out_snapshot)
{
    if (provider == NULL || profile == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot)); (void)snprintf(out_snapshot->provider_id,sizeof(out_snapshot->provider_id),"%s",provider->descriptor.provider_id); (void)snprintf(out_snapshot->profile_id,sizeof(out_snapshot->profile_id),"%s",profile->profile_id); (void)snprintf(out_snapshot->target,sizeof(out_snapshot->target),"%s",profile->target.triple); out_snapshot->family = provider->descriptor.family; out_snapshot->language_mask = provider->descriptor.language_mask; out_snapshot->available = provider->descriptor.available && profile->enabled; out_snapshot->cross_compilation = provider->descriptor.supports_cross_compilation; out_snapshot->response_files = provider->descriptor.supports_response_files; out_snapshot->compilation_database = provider->descriptor.supports_compilation_database; out_snapshot->stable_c_abi = provider->descriptor.supports_stable_c_abi; out_snapshot->debug_information = true; out_snapshot->sanitizers = provider->descriptor.family != UMI_COMPILER_FAMILY_MSVC; out_snapshot->link_time_optimisation = true; return UMI_STATUS_OK;
}
bool umi_compiler_capability_satisfies(const UmiCompilerCapabilitySnapshot *snapshot,UmiCompilerLanguage language,bool require_cross,bool require_c_abi)
{ return snapshot != NULL && snapshot->available && (snapshot->language_mask & UMI_COMPILER_LANGUAGE_BIT(language)) != 0U && (!require_cross || snapshot->cross_compilation) && (!require_c_abi || snapshot->stable_c_abi); }
