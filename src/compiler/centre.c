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
#include "umicom/compiler/centre.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "catalogue_validation.h"

/* Complete nested admission and staged plan/command publication replace unchecked traversal and partial output. Keep the original centre operations for review of selection, defaults and snapshot behavior. The previous implementation is retained for review. */
#if 0
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
#endif
/* A centre owns several independent bounded collections. Admit all of them before
 * traversal; a valid selected row must not conceal damage elsewhere in the owner. */
bool umi_compiler_centre_storage_valid(const UmiCompilerCentre *centre)
{
    if (centre == NULL || centre->profile_count > UMI_COMPILER_MAX_PROFILES ||
        centre->manifest_count > UMI_COMPILER_MAX_MANIFESTS ||
        !UmiCompilerCatalogueText(centre->active_profile_id,sizeof(centre->active_profile_id),false) ||
        !umi_compiler_registry_valid(&centre->providers) ||
        !umi_compiler_artifact_store_valid(&centre->artifacts) ||
        !umi_compiler_cache_valid(&centre->cache) ||
        !umi_polyglot_compilation_database_valid(&centre->compilation_database)) return false;
    for (size_t index = 0U; index < centre->profile_count; ++index)
        if (!umi_compiler_profile_storage_valid(&centre->profiles[index]) ||
            centre->profiles[index].profile_id[0] == '\0') return false;
    for (size_t index = 0U; index < centre->manifest_count; ++index)
        if (!umi_compiler_manifest_storage_valid(&centre->manifests[index]) ||
            centre->manifests[index].project_id[0] == '\0') return false;
    return true;
}
/* Output publication cannot alias an embedded owner field. Subtract addresses
 * instead of adding extents, avoiding end-address overflow on flat-address hosts. */
static bool UmiCompilerCentreOutputOverlaps(const UmiCompilerCentre *centre,const void *output,size_t size)
{
    const uintptr_t owner = (uintptr_t)(const void *)centre, destination = (uintptr_t)output;
    return destination >= owner ? destination - owner < sizeof(*centre) : owner - destination < size;
}
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
    if (centre == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_centre_storage_valid(centre)) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = umi_compiler_profile_validate(profile,&centre->providers,NULL,0U);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < centre->profile_count; ++index)
        if (strcmp(centre->profiles[index].profile_id,profile->profile_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (centre->profile_count == UMI_COMPILER_MAX_PROFILES || centre->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCompilerProfile *destination = &centre->profiles[centre->profile_count];
    (void)memmove(destination,profile,sizeof(*profile));
    if (centre->active_profile_id[0] == '\0')
        (void)memcpy(centre->active_profile_id,destination->profile_id,strlen(destination->profile_id)+1U);
    centre->profile_count += 1U;
    centre->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_compiler_centre_activate_profile(UmiCompilerCentre *centre,const char *profile_id)
{
    if (centre == NULL || !UmiCompilerCatalogueText(profile_id,UMI_COMPILER_ID_CAPACITY,true))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_centre_storage_valid(centre)) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < centre->profile_count; ++index) {
        if (strcmp(centre->profiles[index].profile_id,profile_id) != 0 || !centre->profiles[index].enabled) continue;
        if (centre->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memmove(centre->active_profile_id,profile_id,strlen(profile_id)+1U);
        centre->revision += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
const UmiCompilerProfile *umi_compiler_centre_active_profile(const UmiCompilerCentre *centre)
{
    if (!umi_compiler_centre_storage_valid(centre)) return NULL;
    for (size_t index = 0U; index < centre->profile_count; ++index)
        if (strcmp(centre->profiles[index].profile_id,centre->active_profile_id) == 0) return &centre->profiles[index];
    return NULL;
}
UmiStatus umi_compiler_centre_add_manifest(UmiCompilerCentre *centre,const UmiCompilerProjectManifest *manifest)
{
    if (centre == NULL || manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_centre_storage_valid(centre)) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = umi_compiler_manifest_validate(manifest,NULL,0U);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < centre->manifest_count; ++index)
        if (strcmp(centre->manifests[index].project_id,manifest->project_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (centre->manifest_count == UMI_COMPILER_MAX_MANIFESTS || centre->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memmove(&centre->manifests[centre->manifest_count],manifest,sizeof(*manifest));
    centre->manifest_count += 1U;
    centre->revision += 1U;
    return UMI_STATUS_OK;
}
/* Internal callers have already admitted the complete centre and identifier. */
static const UmiCompilerProjectManifest *find_manifest(const UmiCompilerCentre *centre,const char *project_id)
{
    for (size_t index = 0U; index < centre->manifest_count; ++index)
        if (strcmp(centre->manifests[index].project_id,project_id) == 0) return &centre->manifests[index];
    return NULL;
}
UmiStatus umi_compiler_centre_plan_unit_in_directory(UmiCompilerCentre *centre,const char *project_id,
    const char *unit_id,const char *working_directory,UmiCompilerPlan *out_plan)
{
    if (centre == NULL || out_plan == NULL ||
        !UmiCompilerCatalogueText(project_id,UMI_COMPILER_ID_CAPACITY,true) ||
        !UmiCompilerCatalogueText(unit_id,UMI_COMPILER_ID_CAPACITY,true) ||
        !UmiCompilerCatalogueText(working_directory,UMI_COMPILER_PATH_CAPACITY,true) ||
        UmiCompilerCentreOutputOverlaps(centre,out_plan,sizeof(*out_plan))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_centre_storage_valid(centre)) return UMI_STATUS_INVALID_STATE;
    const UmiCompilerProjectManifest *manifest = find_manifest(centre,project_id);
    if (manifest == NULL) return UMI_STATUS_NOT_FOUND;
    const UmiCompilerUnitManifest *unit = umi_compiler_manifest_find_unit(manifest,unit_id);
    if (unit == NULL) return UMI_STATUS_NOT_FOUND;
    const UmiCompilerProfile *profile = umi_compiler_centre_active_profile(centre);
    if (profile == NULL || !profile->enabled) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = umi_compiler_profile_validate(profile,&centre->providers,NULL,0U);
    if (status != UMI_STATUS_OK) return status;
    if (centre->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiCompilerProvider *provider = unit->provider_id[0] != '\0' ?
        umi_compiler_registry_find(&centre->providers,unit->provider_id) :
        umi_compiler_registry_find(&centre->providers,profile->provider_id);
    /* Preserve the existing fallback selection; it is not a driver availability probe. */
    if (provider == NULL) provider = umi_compiler_registry_resolve(&centre->providers,unit->language,true);
    if (provider == NULL) return UMI_STATUS_UNAVAILABLE;
    UmiCompilerRequest *request = malloc(sizeof(*request));
    UmiPolyglotCompilationEntry *entry = calloc(1U,sizeof(*entry));
    if (request == NULL || entry == NULL) {
        free(entry); free(request); return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = umi_compiler_request_init(request,manifest->project_id,unit->unit_id,unit->language,unit->source,unit->output);
    if (status != UMI_STATUS_OK) goto finish;
    request->target = profile->target;
    const char *standard = unit->language == profile->primary_language ? profile->standard :
        umi_compiler_language_default_standard(unit->language);
    if (!UmiCompilerCatalogueText(standard,sizeof(request->standard),false)) {
        status = UMI_STATUS_INVALID_STATE; goto finish;
    }
    (void)memcpy(request->standard,standard,strlen(standard)+1U);
    (void)memcpy(request->working_directory,working_directory,strlen(working_directory)+1U);
    status = umi_compiler_provider_plan(provider,request,&entry->plan);
    if (status != UMI_STATUS_OK) goto finish;
    status = umi_compiler_policy_append_arguments(&profile->policy,provider->descriptor.family,&entry->plan);
    if (status != UMI_STATUS_OK) goto finish;
    (void)memcpy(entry->unit_id,unit->unit_id,strlen(unit->unit_id)+1U);
    (void)memcpy(entry->directory,working_directory,strlen(working_directory)+1U);
    (void)memcpy(entry->source,unit->source,strlen(unit->source)+1U);
    (void)memcpy(entry->output,unit->output,strlen(unit->output)+1U);
    entry->language = unit->language;
    /* The database service performs its own complete admission and atomic update.
     * After success only infallible copies/rebasing remain. Serialized callers see
     * the returned plan, stored command and centre revision as one operation. */
    status = umi_polyglot_compilation_database_upsert(&centre->compilation_database,entry);
    if (status != UMI_STATUS_OK) goto finish;
    (void)memcpy(out_plan,&entry->plan,sizeof(*out_plan));
    (void)umi_compiler_plan_arguments(out_plan);
    centre->revision += 1U;
finish:
    free(entry); free(request);
    return status;
}
UmiStatus umi_compiler_centre_plan_unit(UmiCompilerCentre *centre,const char *project_id,const char *unit_id,UmiCompilerPlan *out_plan)
{
    /* Compatibility retains the original relative current-directory convention.
     * New clients supply the project directory explicitly through the shared API. */
    return umi_compiler_centre_plan_unit_in_directory(centre,project_id,unit_id,".",out_plan);
}
UmiStatus umi_compiler_centre_snapshot_checked(const UmiCompilerCentre *centre,UmiCompilerCentreSnapshot *out_snapshot)
{
    if (centre == NULL || out_snapshot == NULL ||
        UmiCompilerCentreOutputOverlaps(centre,out_snapshot,sizeof(*out_snapshot))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_centre_storage_valid(centre)) return UMI_STATUS_INVALID_STATE;
    UmiCompilerCentreSnapshot candidate = {0};
    candidate.providers = centre->providers.count;
    for (size_t index = 0U; index < centre->providers.count; ++index)
        if (centre->providers.items[index].descriptor.available) candidate.available_providers += 1U;
    candidate.profiles = centre->profile_count;
    candidate.manifests = centre->manifest_count;
    for (size_t index = 0U; index < centre->manifest_count; ++index)
        candidate.units += centre->manifests[index].unit_count;
    candidate.artifacts = centre->artifacts.count;
    candidate.compilation_commands = centre->compilation_database.count;
    candidate.cache_hits = centre->cache.hits; candidate.cache_misses = centre->cache.misses;
    (void)memcpy(candidate.active_profile_id,centre->active_profile_id,strlen(centre->active_profile_id)+1U);
    candidate.revision = centre->revision;
    (void)memcpy(out_snapshot,&candidate,sizeof(candidate));
    return UMI_STATUS_OK;
}
void umi_compiler_centre_snapshot(const UmiCompilerCentre *centre,UmiCompilerCentreSnapshot *out_snapshot)
{
    if (out_snapshot == NULL || (centre != NULL &&
        UmiCompilerCentreOutputOverlaps(centre,out_snapshot,sizeof(*out_snapshot)))) return;
    if (umi_compiler_centre_snapshot_checked(centre,out_snapshot) != UMI_STATUS_OK)
        (void)memset(out_snapshot,0,sizeof(*out_snapshot));
}


typedef struct UmiCompilerProfilePublication {
    UmiCompilerRegistry providers;
    UmiCompilerProfile profile;
} UmiCompilerProfilePublication;

UmiStatus umi_compiler_centre_configure_profile(UmiCompilerCentre *centre,
    const UmiCompilerProvider *providers,size_t provider_count,
    const UmiCompilerProfile *profile,bool activate)
{
    if (centre == NULL || profile == NULL || (provider_count != 0U && providers == NULL) ||
        provider_count > UMI_COMPILER_MAX_PROVIDERS) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_centre_storage_valid(centre)) return UMI_STATUS_INVALID_STATE;
    if (centre->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Only changed collections need temporary storage. Copying the entire centre
     * would duplicate its much larger database, manifests, artifact and cache owners. */
    UmiCompilerProfilePublication *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    candidate->providers = centre->providers;
    candidate->profile = *profile;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t index = 0U; index < provider_count && status == UMI_STATUS_OK; ++index)
        status = umi_compiler_registry_upsert(&candidate->providers,&providers[index]);
    if (status == UMI_STATUS_OK)
        status = umi_compiler_profile_validate(&candidate->profile,&candidate->providers,NULL,0U);
    if (status == UMI_STATUS_OK && activate && !candidate->profile.enabled)
        status = UMI_STATUS_INVALID_STATE;
    size_t destination = centre->profile_count;
    if (status == UMI_STATUS_OK) {
        for (size_t index = 0U; index < centre->profile_count; ++index)
            if (strcmp(centre->profiles[index].profile_id,candidate->profile.profile_id) == 0) {
                destination = index;
                break;
            }
        if (destination == UMI_COMPILER_MAX_PROFILES) status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (status == UMI_STATUS_OK) {
        /* After admission, publication has no fallible operation. Inputs may
         * borrow existing centre rows; both have been staged before these writes. */
        centre->providers = candidate->providers;
        centre->profiles[destination] = candidate->profile;
        if (destination == centre->profile_count) centre->profile_count += 1U;
        if (activate)
            (void)memcpy(centre->active_profile_id,candidate->profile.profile_id,
                         strlen(candidate->profile.profile_id)+1U);
        centre->revision += 1U;
    }
    free(candidate);
    return status;
}

/* The human-readable compiler summary is shared between native clients. Prepare
 * all text before publication so a short destination cannot become a partial
 * replacement that a UI or command mistakes for a complete snapshot. */
UmiStatus umi_compiler_centre_format_snapshot(const UmiCompilerCentreSnapshot *snapshot,
    char *out_text,size_t capacity)
{
    if (snapshot == NULL || out_text == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    const uintptr_t owner = (uintptr_t)(const void *)snapshot;
    const uintptr_t output = (uintptr_t)(void *)out_text;
    if (output >= owner ? output - owner < sizeof(*snapshot) : owner - output < capacity)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (snapshot->providers > UMI_COMPILER_MAX_PROVIDERS ||
        snapshot->available_providers > snapshot->providers ||
        snapshot->profiles > UMI_COMPILER_MAX_PROFILES ||
        snapshot->manifests > UMI_COMPILER_MAX_MANIFESTS ||
        snapshot->units > UMI_COMPILER_MAX_MANIFESTS * UMI_COMPILER_MAX_UNITS ||
        snapshot->artifacts > UMI_COMPILER_MAX_ARTIFACTS ||
        snapshot->compilation_commands > UMI_POLYGLOT_DATABASE_MAX ||
        !UmiCompilerCatalogueText(snapshot->active_profile_id,sizeof(snapshot->active_profile_id),false))
        return UMI_STATUS_INVALID_STATE;
    char candidate[1024];
    int written = snprintf(candidate,sizeof(candidate),
        "Umicom Compiler Centre\nProviders: %zu (%zu available)\nProfiles: %zu\n"
        "Projects: %zu\nUnits: %zu\nCompilation commands: %zu\nActive profile: %s\n",
        snapshot->providers,snapshot->available_providers,snapshot->profiles,
        snapshot->manifests,snapshot->units,snapshot->compilation_commands,snapshot->active_profile_id);
    if (written < 0) return UMI_STATUS_IO_ERROR;
    if ((size_t)written >= sizeof(candidate) || (size_t)written >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_text,candidate,(size_t)written+1U);
    return UMI_STATUS_OK;
}
