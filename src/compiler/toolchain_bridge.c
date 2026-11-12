/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/toolchain_bridge.c
 *
 * PURPOSE:
 *   Implement the toolchain bridge behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/toolchain_bridge.h"
#include "umicom/compiler/clang_provider.h"
#include "umicom/compiler/gcc_provider.h"
#include "umicom/compiler/msvc_provider.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "catalogue_validation.h"

/* Selected-tool admission and complete profile publication replace ordinal state acceptance and partial conversion. The previous implementation is retained for review. */
#if 0
/* Provide the selected c tool operation used by this module and its client applications. */
static const UmiToolInfo *selected_c_tool(const UmiToolchainProfile *profile)
{
    const UmiToolInfo *tool = umi_toolchain_profile_c_compiler(profile);
    return tool != NULL && tool->state >= UMI_TOOL_FOUND && tool->path[0] != '\0' ? tool : NULL;
}
/*
 * Provide the provider from tool operation used by this module and its client
 * applications.
 */
static UmiCompilerProvider provider_from_tool(const UmiToolInfo *tool,const char *target)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (tool->kind == UMI_TOOL_CLANG) return umi_compiler_clang_provider(tool->path,target);
    /* Apply this branch only when its contract condition is satisfied. */
    if (tool->kind == UMI_TOOL_MSVC_CL) return umi_compiler_msvc_provider(tool->path,target);
    return umi_compiler_gcc_provider(tool->path,target);
}
/*
 * Provide the compiler registry import toolchain operation used by this module and its
 * client applications.
 */
UmiStatus umi_compiler_registry_import_toolchain(UmiCompilerRegistry *registry,const UmiToolchainProfile *profile,size_t *out_imported)
{
    const UmiToolInfo *tool; UmiCompilerProvider provider; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_imported != NULL) *out_imported = 0U;
    tool = selected_c_tool(profile); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (tool == NULL) return UMI_STATUS_UNAVAILABLE;
    provider = provider_from_tool(tool,profile->target_triple); status = umi_compiler_registry_register(registry,&provider); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (status == UMI_STATUS_OK && out_imported != NULL) *out_imported = 1U; return status;
}
/*
 * Provide the compiler profile from toolchain operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_profile_from_toolchain(const UmiToolchainProfile *toolchain,const char *profile_id,UmiCompilerProfile *out_profile)
{
    const UmiToolInfo *tool; UmiCompilerTarget target; UmiCompilerProvider provider; UmiStatus status; const char *target_text;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (toolchain == NULL || profile_id == NULL || out_profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    tool = selected_c_tool(toolchain);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tool == NULL) return UMI_STATUS_UNAVAILABLE;
    target_text = toolchain->target_triple[0] != '\0' ? toolchain->target_triple : NULL; status = target_text != NULL ? umi_compiler_target_parse(target_text,&target) : umi_compiler_target_host(&target); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (status != UMI_STATUS_OK) return status;
    provider = provider_from_tool(tool,target.triple); status = umi_compiler_profile_init(out_profile,profile_id,toolchain->display_name,provider.descriptor.provider_id,UMI_COMPILER_LANGUAGE_C,&target); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    (void)snprintf(out_profile->linker.driver,sizeof(out_profile->linker.driver),"%s",tool->path); return UMI_STATUS_OK;
}

#endif
/* The bridge consumes only compatible profile metadata and one explicitly
 * selected C compiler. FOUND is a reported discovery state, not a new probe.
 * FAILED is a separate state and must never be admitted by ordinal comparison. */
static UmiStatus selected_c_tool(const UmiToolchainProfile *profile,const UmiToolInfo **out_tool)
{
    if (!umi_toolchain_profile_storage_compatible(profile) || out_tool == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiCompilerCatalogueText(profile->display_name,sizeof(profile->display_name),true) ||
        !UmiCompilerCatalogueText(profile->target_triple,sizeof(profile->target_triple),false))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiToolKind kind = profile->selected_c_compiler;
    if (kind != UMI_TOOL_GCC && kind != UMI_TOOL_CLANG && kind != UMI_TOOL_MSVC_CL)
        return UMI_STATUS_NOT_IMPLEMENTED;
    const UmiToolInfo *tool = umi_toolchain_profile_c_compiler(profile);
    if (tool == NULL || tool->kind != kind ||
        !UmiCompilerCatalogueText(tool->path,sizeof(tool->path),false) ||
        !UmiCompilerCatalogueText(tool->version,sizeof(tool->version),false))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (tool->state == UMI_TOOL_MISSING || tool->state == UMI_TOOL_FAILED || tool->path[0] == '\0')
        return UMI_STATUS_UNAVAILABLE;
    if (tool->state != UMI_TOOL_FOUND && tool->state != UMI_TOOL_VALIDATED)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_tool = tool;
    return UMI_STATUS_OK;
}

static UmiStatus provider_from_toolchain(const UmiToolchainProfile *toolchain,
    UmiCompilerProvider *out_provider,UmiCompilerTarget *out_target,const UmiToolInfo **out_tool)
{
    const UmiToolInfo *tool = NULL;
    UmiStatus status = selected_c_tool(toolchain,&tool);
    if (status != UMI_STATUS_OK) return status;
    status = toolchain->target_triple[0] != '\0' ?
        umi_compiler_target_parse(toolchain->target_triple,out_target) : umi_compiler_target_host(out_target);
    if (status != UMI_STATUS_OK) return status;
    UmiCompilerProvider provider;
    if (tool->kind == UMI_TOOL_CLANG) provider = umi_compiler_clang_provider(tool->path,out_target->triple);
    else if (tool->kind == UMI_TOOL_MSVC_CL) provider = umi_compiler_msvc_provider(tool->path,out_target->triple);
    else provider = umi_compiler_gcc_provider(tool->path,out_target->triple);
    if (provider.revision == 0U) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_provider = provider;
    if (out_tool != NULL) *out_tool = tool;
    return UMI_STATUS_OK;
}

UmiStatus umi_compiler_registry_import_toolchain(UmiCompilerRegistry *registry,
    const UmiToolchainProfile *profile,size_t *out_imported)
{
    if (registry == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the established count-output convention and add-only semantics.
     * Callers needing coherent replacement use centre_sync_toolchain instead. */
    if (out_imported != NULL) *out_imported = 0U;
    UmiCompilerProvider provider;
    UmiCompilerTarget target;
    UmiStatus status = provider_from_toolchain(profile,&provider,&target,NULL);
    if (status == UMI_STATUS_OK) status = umi_compiler_registry_register(registry,&provider);
    if (status == UMI_STATUS_OK && out_imported != NULL) *out_imported = 1U;
    return status;
}

UmiStatus umi_compiler_profile_from_toolchain(const UmiToolchainProfile *toolchain,
    const char *profile_id,UmiCompilerProfile *out_profile)
{
    if (toolchain == NULL || out_profile == NULL ||
        !UmiCompilerCatalogueText(profile_id,UMI_COMPILER_ID_CAPACITY,true))
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiToolInfo *tool = NULL;
    UmiCompilerProvider provider;
    UmiCompilerTarget target;
    UmiStatus status = provider_from_toolchain(toolchain,&provider,&target,&tool);
    if (status != UMI_STATUS_OK) return status;
    UmiCompilerProfile *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_compiler_profile_init(candidate,profile_id,toolchain->display_name,
                                       provider.descriptor.provider_id,UMI_COMPILER_LANGUAGE_C,&target);
    if (status == UMI_STATUS_OK &&
        !UmiCompilerCatalogueText(tool->path,sizeof(candidate->linker.driver),true))
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) {
        (void)memcpy(candidate->linker.driver,tool->path,strlen(tool->path)+1U);
        (void)memcpy(out_profile,candidate,sizeof(*candidate));
    }
    free(candidate);
    return status;
}

UmiStatus umi_compiler_centre_sync_toolchain(UmiCompilerCentre *centre,
    const UmiToolchainProfile *toolchain,const char *profile_id)
{
    if (centre == NULL || toolchain == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCompilerProvider provider;
    UmiCompilerTarget target;
    UmiStatus status = provider_from_toolchain(toolchain,&provider,&target,NULL);
    if (status != UMI_STATUS_OK) return status;
    UmiCompilerProfile *profile = malloc(sizeof(*profile));
    if (profile == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_compiler_profile_from_toolchain(toolchain,profile_id,profile);
    if (status == UMI_STATUS_OK)
        status = umi_compiler_centre_configure_profile(centre,&provider,1U,profile,true);
    free(profile);
    return status;
}
