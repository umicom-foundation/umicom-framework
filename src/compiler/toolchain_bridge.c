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
/* Umicom Framework | Toolchain-to-compiler bridge | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/toolchain_bridge.h"
#include "umicom/compiler/clang_provider.h"
#include "umicom/compiler/gcc_provider.h"
#include "umicom/compiler/msvc_provider.h"
#include <stdio.h>
#include <string.h>

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
