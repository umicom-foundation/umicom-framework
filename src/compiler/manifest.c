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
/* Umicom Framework | Polyglot project manifests | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/manifest.h"
#include <stdio.h>
#include <string.h>
/*
 * Initialise compiler manifest from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_compiler_manifest_init(UmiCompilerProjectManifest *manifest,const char *project_id,const char *name,const char *target)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || project_id == NULL || project_id[0] == '\0' || name == NULL || name[0] == '\0' || target == NULL || target[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(manifest,0,sizeof(*manifest)); (void)snprintf(manifest->project_id,sizeof(manifest->project_id),"%s",project_id); (void)snprintf(manifest->name,sizeof(manifest->name),"%s",name); (void)snprintf(manifest->target,sizeof(manifest->target),"%s",target); manifest->revision = 1U; return UMI_STATUS_OK;
}
/*
 * Provide the compiler manifest add unit operation used by this module and its client
 * applications.
 */
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
/*
 * Provide the compiler manifest find unit operation used by this module and its client
 * applications.
 */
const UmiCompilerUnitManifest *umi_compiler_manifest_find_unit(const UmiCompilerProjectManifest *manifest,const char *unit_id)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (manifest == NULL || unit_id == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < manifest->unit_count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(manifest->units[index].unit_id,unit_id) == 0) return &manifest->units[index]; return NULL; }
/* Check that compiler manifest satisfies its contract before another service relies on it. */
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
