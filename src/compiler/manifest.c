/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/manifest.c
 *
 * PURPOSE:
 *   Implement the manifest behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/manifest.h"
#include <stdio.h>
#include <string.h>
#include "catalogue_validation.h"
#include "../base/value_construction_internal.h"

/* Validate all active rows before searching so an early match cannot conceal a
 * malformed later record. Inactive storage is irrelevant to this contract. */
bool umi_compiler_unit_manifest_valid(const UmiCompilerUnitManifest *unit)
{
    return unit != NULL &&
        UmiCompilerCatalogueText(unit->unit_id,sizeof(unit->unit_id),true) &&
        UmiCompilerCatalogueText(unit->source,sizeof(unit->source),true) &&
        UmiCompilerCatalogueText(unit->output,sizeof(unit->output),true) &&
        UmiCompilerCatalogueText(unit->provider_id,sizeof(unit->provider_id),false) &&
        UmiCompilerCatalogueText(unit->exports_abi,sizeof(unit->exports_abi),false) &&
        unit->language > UMI_COMPILER_LANGUAGE_UNKNOWN && unit->language <= UMI_COMPILER_LANGUAGE_UAI;
}

bool umi_compiler_manifest_storage_valid(const UmiCompilerProjectManifest *manifest)
{
    if (manifest == NULL || manifest->unit_count > UMI_COMPILER_MAX_UNITS ||
        !UmiCompilerCatalogueText(manifest->project_id,sizeof(manifest->project_id),false) ||
        !UmiCompilerCatalogueText(manifest->name,sizeof(manifest->name),false) ||
        !UmiCompilerCatalogueText(manifest->target,sizeof(manifest->target),false)) return false;
    for (size_t i = 0U; i < manifest->unit_count; ++i)
        if (!umi_compiler_unit_manifest_valid(&manifest->units[i])) return false;
    return true;
}

/*
 * Initialise compiler manifest from caller-provided values so later operations receive a
 * known state.
 */
/* Use the shared heap-staged constructor for this large inline owner so bounded identities and borrowed input survive until successful publication. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_manifest_init(UmiCompilerProjectManifest *manifest,const char *project_id,const char *name,const char *target)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || project_id == NULL || project_id[0] == '\0' || name == NULL || name[0] == '\0' || target == NULL || target[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(manifest,0,sizeof(*manifest)); (void)snprintf(manifest->project_id,sizeof(manifest->project_id),"%s",project_id); (void)snprintf(manifest->name,sizeof(manifest->name),"%s",name); (void)snprintf(manifest->target,sizeof(manifest->target),"%s",target); manifest->revision = 1U; return UMI_STATUS_OK;
}
#endif
static UmiStatus UmiCompilerManifestBuild(UmiCompilerProjectManifest *manifest,const char *project_id,const char *name,const char *target)
{
    if (project_id == NULL || name == NULL || target == NULL ||
        project_id[0] == '\0' || name[0] == '\0' || target[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiCompilerCatalogueText(project_id,sizeof(manifest->project_id),true) ||
        !UmiCompilerCatalogueText(name,sizeof(manifest->name),true) ||
        !UmiCompilerCatalogueText(target,sizeof(manifest->target),true)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(manifest,0,sizeof(*manifest));
    (void)memcpy(manifest->project_id,project_id,strlen(project_id) + 1U);
    (void)memcpy(manifest->name,name,strlen(name) + 1U);
    (void)memcpy(manifest->target,target,strlen(target) + 1U);
    manifest->revision = 1U;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_compiler_manifest_init,UmiCompilerProjectManifest,manifest,
    (UmiCompilerProjectManifest *manifest,const char *project_id,const char *name,const char *target),
    UmiCompilerManifestBuild(candidate,project_id,name,target))
/*
 * Provide the compiler manifest add unit operation used by this module and its client
 * applications.
 */
/* Admit the complete owner and incoming row before duplicate lookup, and refuse slot or revision exhaustion before any publication. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_manifest_add_unit(UmiCompilerProjectManifest *manifest,const UmiCompilerUnitManifest *unit)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || unit == NULL || unit->unit_id[0] == '\0' || unit->source[0] == '\0' || unit->output[0] == '\0' || unit->language == UMI_COMPILER_LANGUAGE_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manifest->unit_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(manifest->units[index].unit_id,unit->unit_id) == 0 || strcmp(manifest->units[index].source,unit->source) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (manifest->unit_count >= UMI_COMPILER_MAX_UNITS) return UMI_STATUS_CAPACITY_EXCEEDED;
    manifest->units[manifest->unit_count++] = *unit; manifest->revision += 1U; return UMI_STATUS_OK;
}
#endif
UmiStatus umi_compiler_manifest_add_unit(UmiCompilerProjectManifest *manifest,const UmiCompilerUnitManifest *unit)
{
    if (manifest == NULL || !umi_compiler_unit_manifest_valid(unit)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_manifest_storage_valid(manifest)) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < manifest->unit_count; ++i)
        if (strcmp(manifest->units[i].unit_id,unit->unit_id) == 0 ||
            strcmp(manifest->units[i].source,unit->source) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (manifest->unit_count == UMI_COMPILER_MAX_UNITS || manifest->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Admission precedes every write; an incoming row may be the unused
     * destination slot itself. memmove retains that borrowed-input behavior. */
    (void)memmove(&manifest->units[manifest->unit_count],unit,sizeof(*unit));
    manifest->unit_count += 1U;
    manifest->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the compiler manifest find unit operation used by this module and its client
 * applications.
 */
/* Bound lookup input and every active record before returning a borrowed match; revision exhaustion does not prevent valid reads. The previous implementation is retained for review. */
#if 0
const UmiCompilerUnitManifest *umi_compiler_manifest_find_unit(const UmiCompilerProjectManifest *manifest,const char *unit_id)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (manifest == NULL || unit_id == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < manifest->unit_count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(manifest->units[index].unit_id,unit_id) == 0) return &manifest->units[index]; return NULL; }
#endif
const UmiCompilerUnitManifest *umi_compiler_manifest_find_unit(const UmiCompilerProjectManifest *manifest,const char *unit_id)
{
    if (!UmiCompilerCatalogueText(unit_id,UMI_COMPILER_ID_CAPACITY,true) ||
        !umi_compiler_manifest_storage_valid(manifest)) return NULL;
    for (size_t i = 0U; i < manifest->unit_count; ++i)
        if (strcmp(manifest->units[i].unit_id,unit_id) == 0) return &manifest->units[i];
    return NULL;
}
/* Check that compiler manifest satisfies its contract before another service relies on it. */
/* Validate bounded storage before inspecting required metadata and entry points, preserving the existing semantic and optional diagnostic behavior. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_manifest_validate(const UmiCompilerProjectManifest *manifest,char *out_reason,size_t capacity)
{
    const char *reason = "ok"; UmiStatus status = UMI_STATUS_OK; size_t index,entries = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (manifest->project_id[0] == '\0' || manifest->target[0] == '\0') { reason = "project identity and target are required"; status = UMI_STATUS_INVALID_ARGUMENT; }
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (manifest->unit_count == 0U) { reason = "at least one compilation unit is required"; status = UMI_STATUS_INVALID_STATE; }
    /* Use this fallback path when the earlier condition does not apply. */
    else { /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < manifest->unit_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (manifest->units[index].entry_point) entries += 1U; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (entries > 1U) { reason = "only one entry point is permitted"; status = UMI_STATUS_INVALID_STATE; } }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_reason != NULL && capacity > 0U) (void)snprintf(out_reason,capacity,"%s",reason);
    return status;
}
#endif
UmiStatus umi_compiler_manifest_validate(const UmiCompilerProjectManifest *manifest,char *out_reason,size_t capacity)
{
    if (manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const char *reason = "ok";
    UmiStatus status = UMI_STATUS_OK;
    if (!umi_compiler_manifest_storage_valid(manifest)) {
        reason = "manifest storage or compilation unit is malformed";
        status = UMI_STATUS_INVALID_STATE;
    } else if (manifest->project_id[0] == '\0' || manifest->target[0] == '\0') {
        reason = "project identity and target are required";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (manifest->unit_count == 0U) {
        reason = "at least one compilation unit is required";
        status = UMI_STATUS_INVALID_STATE;
    } else {
        size_t entries = 0U;
        for (size_t i = 0U; i < manifest->unit_count; ++i)
            if (manifest->units[i].entry_point) entries += 1U;
        if (entries > 1U) {
            reason = "only one entry point is permitted";
            status = UMI_STATUS_INVALID_STATE;
        }
    }
    /* Preserve the established optional diagnostic contract: a small buffer
     * receives a terminated prefix without changing the semantic status. */
    if (out_reason != NULL && capacity > 0U) (void)snprintf(out_reason,capacity,"%s",reason);
    return status;
}

/* Language projection belongs beside manifest admission so clients never shift
 * an unchecked enum or traverse an unchecked count when composing their views. */
UmiStatus umi_compiler_manifest_languages(const UmiCompilerProjectManifest *manifest,
    uint32_t *out_mask)
{
    if (manifest == NULL || out_mask == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const uintptr_t owner = (uintptr_t)(const void *)manifest;
    const uintptr_t output = (uintptr_t)(void *)out_mask;
    if (output >= owner ? output - owner < sizeof(*manifest) : owner - output < sizeof(*out_mask))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_manifest_storage_valid(manifest)) return UMI_STATUS_INVALID_STATE;
    uint32_t mask = 0U;
    for (size_t index = 0U; index < manifest->unit_count; ++index)
        mask |= UMI_COMPILER_LANGUAGE_BIT(manifest->units[index].language);
    *out_mask = mask;
    return UMI_STATUS_OK;
}
