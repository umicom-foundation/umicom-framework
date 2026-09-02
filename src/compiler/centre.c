/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/centre.c
 *
 * PURPOSE:
 *   Implement the centre behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Compiler and polyglot centre | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/centre.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>

/*
 * Initialise compiler centre from caller-provided values so later operations receive a
 * known state.
 */
void umi_compiler_centre_init(UmiCompilerCentre *centre)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL) return;
    (void)memset(centre,0,sizeof(*centre));
    umi_compiler_registry_init(&centre->providers);
    centre->artifacts.revision = 1U;
    centre->cache.revision = 1U;
    centre->compilation_database.revision = 1U;
    centre->revision = 1U;
}
/*
 * Provide the compiler centre add profile operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_centre_add_profile(UmiCompilerCentre *centre,const UmiCompilerProfile *profile)
{
    size_t index; UmiStatus status; char reason[UMI_COMPILER_TEXT_CAPACITY];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_compiler_profile_validate(profile,&centre->providers,reason,sizeof(reason));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < centre->profile_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(centre->profiles[index].profile_id,profile->profile_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (centre->profile_count >= UMI_COMPILER_MAX_PROFILES) return UMI_STATUS_CAPACITY_EXCEEDED;
    centre->profiles[centre->profile_count++] = *profile;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (centre->active_profile_id[0] == '\0') (void)snprintf(centre->active_profile_id,sizeof(centre->active_profile_id),"%s",profile->profile_id);
    centre->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the compiler centre activate profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_compiler_centre_activate_profile(UmiCompilerCentre *centre,const char *profile_id)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (centre == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < centre->profile_count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(centre->profiles[index].profile_id,profile_id) == 0 && centre->profiles[index].enabled) { (void)snprintf(centre->active_profile_id,sizeof(centre->active_profile_id),"%s",profile_id); centre->revision += 1U; return UMI_STATUS_OK; } return UMI_STATUS_NOT_FOUND; }
/*
 * Provide the compiler centre active profile operation used by this module and its client
 * applications.
 */
const UmiCompilerProfile *umi_compiler_centre_active_profile(const UmiCompilerCentre *centre)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (centre == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < centre->profile_count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(centre->profiles[index].profile_id,centre->active_profile_id) == 0) return &centre->profiles[index]; return NULL; }
/*
 * Provide the compiler centre add manifest operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_centre_add_manifest(UmiCompilerCentre *centre,const UmiCompilerProjectManifest *manifest)
{
    size_t index; UmiStatus status; char reason[UMI_COMPILER_TEXT_CAPACITY];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL || manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_compiler_manifest_validate(manifest,reason,sizeof(reason));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < centre->manifest_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(centre->manifests[index].project_id,manifest->project_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (centre->manifest_count >= UMI_COMPILER_MAX_MANIFESTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    centre->manifests[centre->manifest_count++] = *manifest;
    centre->revision += 1U;
    return UMI_STATUS_OK;
}
/* Provide the find manifest operation used by this module and its client applications. */
static const UmiCompilerProjectManifest *find_manifest(const UmiCompilerCentre *centre,const char *project_id)
{ size_t index; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < centre->manifest_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(centre->manifests[index].project_id,project_id) == 0) return &centre->manifests[index]; return NULL; }
/*
 * Provide the compiler centre plan unit operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_centre_plan_unit(UmiCompilerCentre *centre,const char *project_id,const char *unit_id,UmiCompilerPlan *out_plan)
{
    const UmiCompilerProjectManifest *manifest; const UmiCompilerUnitManifest *unit; const UmiCompilerProfile *profile; const UmiCompilerProvider *provider; UmiCompilerRequest request; UmiPolyglotCompilationEntry entry; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL || project_id == NULL || unit_id == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    manifest = find_manifest(centre,project_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (manifest == NULL) return UMI_STATUS_NOT_FOUND; unit = umi_compiler_manifest_find_unit(manifest,unit_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (unit == NULL) return UMI_STATUS_NOT_FOUND; profile = umi_compiler_centre_active_profile(centre); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (profile == NULL) return UMI_STATUS_INVALID_STATE;
    provider = unit->provider_id[0] != '\0' ? umi_compiler_registry_find(&centre->providers,unit->provider_id) : umi_compiler_registry_find(&centre->providers,profile->provider_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (provider == NULL) provider = umi_compiler_registry_resolve(&centre->providers,unit->language,true); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (provider == NULL) return UMI_STATUS_UNAVAILABLE;
    status = umi_compiler_request_init(&request,manifest->project_id,unit->unit_id,unit->language,unit->source,unit->output); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status; request.target = profile->target; (void)snprintf(request.standard,sizeof(request.standard),"%s",unit->language == profile->primary_language ? profile->standard : umi_compiler_language_default_standard(unit->language));
    status = umi_compiler_provider_plan(provider,&request,out_plan); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status; status = umi_compiler_policy_append_arguments(&profile->policy,provider->descriptor.family,out_plan); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    (void)memset(&entry,0,sizeof(entry)); (void)snprintf(entry.unit_id,sizeof(entry.unit_id),"%s",unit->unit_id); (void)snprintf(entry.directory,sizeof(entry.directory),"."); (void)snprintf(entry.source,sizeof(entry.source),"%s",unit->source); (void)snprintf(entry.output,sizeof(entry.output),"%s",unit->output); entry.language = unit->language; entry.plan = *out_plan; status = umi_polyglot_compilation_database_add(&centre->compilation_database,&entry); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK && status != UMI_STATUS_ALREADY_EXISTS) return status; centre->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the compiler centre snapshot operation used by this module and its client
 * applications.
 */
void umi_compiler_centre_snapshot(const UmiCompilerCentre *centre,UmiCompilerCentreSnapshot *out_snapshot)
{
    size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (out_snapshot == NULL) return; (void)memset(out_snapshot,0,sizeof(*out_snapshot)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (centre == NULL) return; out_snapshot->providers = centre->providers.count; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < centre->providers.count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (centre->providers.items[index].descriptor.available) out_snapshot->available_providers += 1U; out_snapshot->profiles = centre->profile_count; out_snapshot->manifests = centre->manifest_count; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < centre->manifest_count; ++index) out_snapshot->units += centre->manifests[index].unit_count; out_snapshot->artifacts = centre->artifacts.count; out_snapshot->compilation_commands = centre->compilation_database.count; out_snapshot->cache_hits = centre->cache.hits; out_snapshot->cache_misses = centre->cache.misses; (void)snprintf(out_snapshot->active_profile_id,sizeof(out_snapshot->active_profile_id),"%s",centre->active_profile_id); out_snapshot->revision = centre->revision;
}
