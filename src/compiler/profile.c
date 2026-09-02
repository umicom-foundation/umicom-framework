/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/profile.c
 *
 * PURPOSE:
 *   Implement the profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Compiler profiles | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/profile.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>
/*
 * Initialise compiler profile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_compiler_profile_init(UmiCompilerProfile *profile,const char *profile_id,const char *display_name,const char *provider_id,UmiCompilerLanguage language,const UmiCompilerTarget *target)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || profile_id == NULL || profile_id[0] == '\0' || display_name == NULL || display_name[0] == '\0' || provider_id == NULL || provider_id[0] == '\0' || target == NULL || language == UMI_COMPILER_LANGUAGE_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(profile,0,sizeof(*profile)); (void)snprintf(profile->profile_id,sizeof(profile->profile_id),"%s",profile_id); (void)snprintf(profile->display_name,sizeof(profile->display_name),"%s",display_name); (void)snprintf(profile->provider_id,sizeof(profile->provider_id),"%s",provider_id); (void)snprintf(profile->standard,sizeof(profile->standard),"%s",umi_compiler_language_default_standard(language)); profile->primary_language = language; profile->target = *target; profile->policy = umi_compiler_policy_debug(); profile->enabled = true; profile->revision = 1U; return umi_linker_profile_init(&profile->linker,profile_id,provider_id,target);
}
/* Check that compiler profile satisfies its contract before another service relies on it. */
UmiStatus umi_compiler_profile_validate(const UmiCompilerProfile *profile,const UmiCompilerRegistry *registry,char *out_reason,size_t capacity)
{
    const char *reason = "ok"; UmiStatus status = UMI_STATUS_OK; const UmiCompilerProvider *provider;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    provider = umi_compiler_registry_find(registry,profile->provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (profile->profile_id[0] == '\0' || profile->target.triple[0] == '\0') { reason = "profile identity and target are required"; status = UMI_STATUS_INVALID_ARGUMENT; }
    else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (provider == NULL) { reason = "compiler provider is not registered"; status = UMI_STATUS_NOT_FOUND; }
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (!umi_compiler_provider_supports(provider,profile->primary_language)) { reason = "provider does not support the primary language"; status = UMI_STATUS_NOT_IMPLEMENTED; }
    /* Use this fallback path when the earlier condition does not apply. */
    else status = umi_compiler_policy_validate(&profile->policy,NULL,0U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_reason != NULL && capacity > 0U) (void)snprintf(out_reason,capacity,"%s",reason);
    return status;
}
