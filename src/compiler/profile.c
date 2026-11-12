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
#include "umicom/compiler/profile.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>
#include "catalogue_validation.h"
#include "../base/value_construction_internal.h"

/* Profile text and its nested linker owner must be bounded before any provider
 * lookup. Execution policy and registered-language support remain separate. */
bool umi_compiler_profile_storage_valid(const UmiCompilerProfile *profile)
{
    return profile != NULL &&
        UmiCompilerCatalogueText(profile->profile_id,sizeof(profile->profile_id),false) &&
        UmiCompilerCatalogueText(profile->display_name,sizeof(profile->display_name),false) &&
        UmiCompilerCatalogueText(profile->provider_id,sizeof(profile->provider_id),false) &&
        UmiCompilerCatalogueText(profile->standard,sizeof(profile->standard),false) &&
        UmiCompilerCatalogueTarget(&profile->target) &&
        umi_linker_profile_storage_valid(&profile->linker);
}

/*
 * Initialise compiler profile from caller-provided values so later operations receive a
 * known state.
 */
/* Construct the complete inline profile off-stack before publication, including its fallible nested linker initialization. This preserves previous values and aliased input. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_profile_init(UmiCompilerProfile *profile,const char *profile_id,const char *display_name,const char *provider_id,UmiCompilerLanguage language,const UmiCompilerTarget *target)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || profile_id == NULL || profile_id[0] == '\0' || display_name == NULL || display_name[0] == '\0' || provider_id == NULL || provider_id[0] == '\0' || target == NULL || language == UMI_COMPILER_LANGUAGE_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(profile,0,sizeof(*profile)); (void)snprintf(profile->profile_id,sizeof(profile->profile_id),"%s",profile_id); (void)snprintf(profile->display_name,sizeof(profile->display_name),"%s",display_name); (void)snprintf(profile->provider_id,sizeof(profile->provider_id),"%s",provider_id); (void)snprintf(profile->standard,sizeof(profile->standard),"%s",umi_compiler_language_default_standard(language)); profile->primary_language = language; profile->target = *target; profile->policy = umi_compiler_policy_debug(); profile->enabled = true; profile->revision = 1U; return umi_linker_profile_init(&profile->linker,profile_id,provider_id,target);
}
#endif
static UmiStatus UmiCompilerProfileBuild(UmiCompilerProfile *profile,const char *profile_id,const char *display_name,const char *provider_id,UmiCompilerLanguage language,const UmiCompilerTarget *target)
{
    if (profile_id == NULL || display_name == NULL || provider_id == NULL ||
        profile_id[0] == '\0' || display_name[0] == '\0' || provider_id[0] == '\0' ||
        !UmiCompilerCatalogueTarget(target) ||
        language <= UMI_COMPILER_LANGUAGE_UNKNOWN || language > UMI_COMPILER_LANGUAGE_UAI)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *standard = umi_compiler_language_default_standard(language);
    if (!UmiCompilerCatalogueText(profile_id,sizeof(profile->profile_id),true) ||
        !UmiCompilerCatalogueText(display_name,sizeof(profile->display_name),true) ||
        !UmiCompilerCatalogueText(provider_id,sizeof(profile->provider_id),true) ||
        !UmiCompilerCatalogueText(standard,sizeof(profile->standard),false))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(profile,0,sizeof(*profile));
    (void)memcpy(profile->profile_id,profile_id,strlen(profile_id) + 1U);
    (void)memcpy(profile->display_name,display_name,strlen(display_name) + 1U);
    (void)memcpy(profile->provider_id,provider_id,strlen(provider_id) + 1U);
    (void)memcpy(profile->standard,standard,strlen(standard) + 1U);
    profile->primary_language = language;
    profile->target = *target;
    profile->policy = umi_compiler_policy_debug();
    profile->enabled = true;
    profile->revision = 1U;
    return umi_linker_profile_init(&profile->linker,profile_id,provider_id,target);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_compiler_profile_init,UmiCompilerProfile,profile,
    (UmiCompilerProfile *profile,const char *profile_id,const char *display_name,const char *provider_id,UmiCompilerLanguage language,const UmiCompilerTarget *target),
    UmiCompilerProfileBuild(candidate,profile_id,display_name,provider_id,language,target))
/* Check that compiler profile satisfies its contract before another service relies on it. */
/* Admit all profile and registry storage before lookups, retain provider/policy selection, and carry a policy refusal reason to the caller. The previous implementation is retained for review. */
#if 0
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
#endif
UmiStatus umi_compiler_profile_validate(const UmiCompilerProfile *profile,const UmiCompilerRegistry *registry,char *out_reason,size_t capacity)
{
    if (profile == NULL || registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const char *reason = "ok";
    UmiStatus status = UMI_STATUS_OK;
    char policy_reason[UMI_COMPILER_TEXT_CAPACITY];
    if (!umi_compiler_profile_storage_valid(profile) || !umi_compiler_registry_valid(registry)) {
        reason = "profile or provider registry storage is malformed";
        status = UMI_STATUS_INVALID_STATE;
    } else if (profile->profile_id[0] == '\0' || profile->target.triple[0] == '\0') {
        reason = "profile identity and target are required";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (profile->primary_language <= UMI_COMPILER_LANGUAGE_UNKNOWN ||
               profile->primary_language > UMI_COMPILER_LANGUAGE_UAI) {
        reason = "primary language is unknown";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else {
        const UmiCompilerProvider *provider = umi_compiler_registry_find(registry,profile->provider_id);
        if (provider == NULL) {
            reason = "compiler provider is not registered";
            status = UMI_STATUS_NOT_FOUND;
        } else if (!umi_compiler_provider_supports(provider,profile->primary_language)) {
            reason = "provider does not support the primary language";
            status = UMI_STATUS_NOT_IMPLEMENTED;
        } else {
            status = umi_compiler_policy_validate(&profile->policy,policy_reason,sizeof(policy_reason));
            if (status != UMI_STATUS_OK) reason = policy_reason;
        }
    }
    /* Keep the optional diagnostic's truncated-prefix behavior, but propagate
     * the policy reason instead of describing a policy refusal as "ok". */
    if (out_reason != NULL && capacity > 0U) (void)snprintf(out_reason,capacity,"%s",reason);
    return status;
}
