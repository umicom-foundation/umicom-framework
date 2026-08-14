/* Umicom Framework | Compiler and polyglot centre | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/centre.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>

void umi_compiler_centre_init(UmiCompilerCentre *centre)
{
    if (centre == NULL) return;
    (void)memset(centre,0,sizeof(*centre));
    umi_compiler_registry_init(&centre->providers);
    centre->artifacts.revision = 1U;
    centre->cache.revision = 1U;
    centre->compilation_database.revision = 1U;
    centre->revision = 1U;
}
UmiStatus umi_compiler_centre_add_profile(UmiCompilerCentre *centre,const UmiCompilerProfile *profile)
{
    size_t index; UmiStatus status; char reason[UMI_COMPILER_TEXT_CAPACITY];
    if (centre == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_compiler_profile_validate(profile,&centre->providers,reason,sizeof(reason));
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < centre->profile_count; ++index) if (strcmp(centre->profiles[index].profile_id,profile->profile_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (centre->profile_count >= UMI_COMPILER_MAX_PROFILES) return UMI_STATUS_CAPACITY_EXCEEDED;
    centre->profiles[centre->profile_count++] = *profile;
    if (centre->active_profile_id[0] == '\0') (void)snprintf(centre->active_profile_id,sizeof(centre->active_profile_id),"%s",profile->profile_id);
    centre->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_compiler_centre_activate_profile(UmiCompilerCentre *centre,const char *profile_id)
{ size_t index; if (centre == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT; for (index = 0U; index < centre->profile_count; ++index) if (strcmp(centre->profiles[index].profile_id,profile_id) == 0 && centre->profiles[index].enabled) { (void)snprintf(centre->active_profile_id,sizeof(centre->active_profile_id),"%s",profile_id); centre->revision += 1U; return UMI_STATUS_OK; } return UMI_STATUS_NOT_FOUND; }
const UmiCompilerProfile *umi_compiler_centre_active_profile(const UmiCompilerCentre *centre)
{ size_t index; if (centre == NULL) return NULL; for (index = 0U; index < centre->profile_count; ++index) if (strcmp(centre->profiles[index].profile_id,centre->active_profile_id) == 0) return &centre->profiles[index]; return NULL; }
UmiStatus umi_compiler_centre_add_manifest(UmiCompilerCentre *centre,const UmiCompilerProjectManifest *manifest)
{
    size_t index; UmiStatus status; char reason[UMI_COMPILER_TEXT_CAPACITY];
    if (centre == NULL || manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_compiler_manifest_validate(manifest,reason,sizeof(reason));
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < centre->manifest_count; ++index) if (strcmp(centre->manifests[index].project_id,manifest->project_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (centre->manifest_count >= UMI_COMPILER_MAX_MANIFESTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    centre->manifests[centre->manifest_count++] = *manifest;
    centre->revision += 1U;
    return UMI_STATUS_OK;
}
static const UmiCompilerProjectManifest *find_manifest(const UmiCompilerCentre *centre,const char *project_id)
{ size_t index; for (index = 0U; index < centre->manifest_count; ++index) if (strcmp(centre->manifests[index].project_id,project_id) == 0) return &centre->manifests[index]; return NULL; }
UmiStatus umi_compiler_centre_plan_unit(UmiCompilerCentre *centre,const char *project_id,const char *unit_id,UmiCompilerPlan *out_plan)
{
    const UmiCompilerProjectManifest *manifest; const UmiCompilerUnitManifest *unit; const UmiCompilerProfile *profile; const UmiCompilerProvider *provider; UmiCompilerRequest request; UmiPolyglotCompilationEntry entry; UmiStatus status;
    if (centre == NULL || project_id == NULL || unit_id == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    manifest = find_manifest(centre,project_id); if (manifest == NULL) return UMI_STATUS_NOT_FOUND; unit = umi_compiler_manifest_find_unit(manifest,unit_id); if (unit == NULL) return UMI_STATUS_NOT_FOUND; profile = umi_compiler_centre_active_profile(centre); if (profile == NULL) return UMI_STATUS_INVALID_STATE;
    provider = unit->provider_id[0] != '\0' ? umi_compiler_registry_find(&centre->providers,unit->provider_id) : umi_compiler_registry_find(&centre->providers,profile->provider_id); if (provider == NULL) provider = umi_compiler_registry_resolve(&centre->providers,unit->language,true); if (provider == NULL) return UMI_STATUS_UNAVAILABLE;
    status = umi_compiler_request_init(&request,manifest->project_id,unit->unit_id,unit->language,unit->source,unit->output); if (status != UMI_STATUS_OK) return status; request.target = profile->target; (void)snprintf(request.standard,sizeof(request.standard),"%s",unit->language == profile->primary_language ? profile->standard : umi_compiler_language_default_standard(unit->language));
    status = umi_compiler_provider_plan(provider,&request,out_plan); if (status != UMI_STATUS_OK) return status; status = umi_compiler_policy_append_arguments(&profile->policy,provider->descriptor.family,out_plan); if (status != UMI_STATUS_OK) return status;
    (void)memset(&entry,0,sizeof(entry)); (void)snprintf(entry.unit_id,sizeof(entry.unit_id),"%s",unit->unit_id); (void)snprintf(entry.directory,sizeof(entry.directory),"."); (void)snprintf(entry.source,sizeof(entry.source),"%s",unit->source); (void)snprintf(entry.output,sizeof(entry.output),"%s",unit->output); entry.language = unit->language; entry.plan = *out_plan; status = umi_polyglot_compilation_database_add(&centre->compilation_database,&entry); if (status != UMI_STATUS_OK && status != UMI_STATUS_ALREADY_EXISTS) return status; centre->revision += 1U; return UMI_STATUS_OK;
}
void umi_compiler_centre_snapshot(const UmiCompilerCentre *centre,UmiCompilerCentreSnapshot *out_snapshot)
{
    size_t index; if (out_snapshot == NULL) return; (void)memset(out_snapshot,0,sizeof(*out_snapshot)); if (centre == NULL) return; out_snapshot->providers = centre->providers.count; for (index = 0U; index < centre->providers.count; ++index) if (centre->providers.items[index].descriptor.available) out_snapshot->available_providers += 1U; out_snapshot->profiles = centre->profile_count; out_snapshot->manifests = centre->manifest_count; for (index = 0U; index < centre->manifest_count; ++index) out_snapshot->units += centre->manifests[index].unit_count; out_snapshot->artifacts = centre->artifacts.count; out_snapshot->compilation_commands = centre->compilation_database.count; out_snapshot->cache_hits = centre->cache.hits; out_snapshot->cache_misses = centre->cache.misses; (void)snprintf(out_snapshot->active_profile_id,sizeof(out_snapshot->active_profile_id),"%s",centre->active_profile_id); out_snapshot->revision = centre->revision;
}
