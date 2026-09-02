/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/provider.c
 *
 * PURPOSE:
 *   Implement the provider behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Compiler providers | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/provider.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>
/* Provide the add formatted operation used by this module and its client applications. */
static UmiStatus add_formatted(UmiCompilerPlan *plan,const char *format,const char *value)
{ char buffer[UMI_COMPILER_TEXT_CAPACITY]; int length = snprintf(buffer,sizeof(buffer),format,value); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (length < 0 || (size_t)length >= sizeof(buffer)) return UMI_STATUS_CAPACITY_EXCEEDED; return umi_compiler_plan_add_argument(plan,buffer); }
/*
 * Initialise compiler provider from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_compiler_provider_init(UmiCompilerProvider *provider,const UmiCompilerProviderDescriptor *descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL || descriptor == NULL || descriptor->struct_size < sizeof(*descriptor) || descriptor->api_version != UMI_COMPILER_API_VERSION || descriptor->provider_id[0] == '\0' || descriptor->display_name[0] == '\0' || descriptor->family == UMI_COMPILER_FAMILY_UNKNOWN || descriptor->language_mask == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(provider,0,sizeof(*provider)); provider->descriptor = *descriptor; provider->revision = 1U; return UMI_STATUS_OK;
}
/*
 * Provide the compiler provider supports operation used by this module and its client
 * applications.
 */
bool umi_compiler_provider_supports(const UmiCompilerProvider *provider,UmiCompilerLanguage language) { return provider != NULL && language > UMI_COMPILER_LANGUAGE_UNKNOWN && language <= UMI_COMPILER_LANGUAGE_UAI && (provider->descriptor.language_mask & UMI_COMPILER_LANGUAGE_BIT(language)) != 0U; }
/*
 * Provide the compiler provider plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_provider_plan(const UmiCompilerProvider *provider,const UmiCompilerRequest *request,UmiCompilerPlan *out_plan)
{
    UmiStatus status; char reason[UMI_COMPILER_TEXT_CAPACITY]; size_t index; bool msvc;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL || request == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_compiler_request_validate(request,reason,sizeof(reason)); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_compiler_provider_supports(provider,request->language)) return UMI_STATUS_NOT_IMPLEMENTED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!provider->descriptor.available) return UMI_STATUS_UNAVAILABLE;
    umi_compiler_plan_init(out_plan);
    (void)snprintf(out_plan->provider_id,sizeof(out_plan->provider_id),"%s",provider->descriptor.provider_id);
    (void)snprintf(out_plan->executable,sizeof(out_plan->executable),"%s",provider->descriptor.executable);
    (void)snprintf(out_plan->working_directory,sizeof(out_plan->working_directory),"%s",request->working_directory);
    out_plan->language = request->language; out_plan->action = request->action; msvc = provider->descriptor.family == UMI_COMPILER_FAMILY_MSVC;
    /* Apply this branch only when its contract condition is satisfied. */
    if (provider->descriptor.family == UMI_COMPILER_FAMILY_RUSTC) {
        status = umi_compiler_plan_add_argument(out_plan,request->source_path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = add_formatted(out_plan,"--edition=%s",request->standard); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,"--target"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,request->target.triple); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,"-o"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,request->output_path);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (provider->descriptor.family == UMI_COMPILER_FAMILY_ZIG) {
        status = umi_compiler_plan_add_argument(out_plan,"build-obj"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,request->source_path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,"-target"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,request->target.triple); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = add_formatted(out_plan,"-femit-bin=%s",request->output_path);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (provider->descriptor.family == UMI_COMPILER_FAMILY_UAI || provider->descriptor.family == UMI_COMPILER_FAMILY_UMICC) {
        status = umi_compiler_plan_add_argument(out_plan,"compile"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,"--language"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,umi_compiler_language_name(request->language)); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,"--target"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,request->target.triple); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,"--output"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,request->output_path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,request->source_path);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = umi_compiler_plan_add_argument(out_plan,msvc ? "/nologo" : "-c"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK && msvc) status = umi_compiler_plan_add_argument(out_plan,"/c"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK && request->language != UMI_COMPILER_LANGUAGE_ASSEMBLY) status = add_formatted(out_plan,msvc ? "/std:%s" : "-std=%s",request->standard); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK && provider->descriptor.family == UMI_COMPILER_FAMILY_CLANG) status = add_formatted(out_plan,"--target=%s",request->target.triple); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = umi_compiler_plan_add_argument(out_plan,request->source_path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) status = add_formatted(out_plan,msvc ? "/Fo%s" : "-o",msvc ? request->output_path : ""); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK && !msvc) status = umi_compiler_plan_add_argument(out_plan,request->output_path);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->include_directory_count; ++index) { status = add_formatted(out_plan,msvc ? "/I%s" : "-I%s",request->include_directories[index]); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status != UMI_STATUS_OK) return status; }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->definition_count; ++index) { status = add_formatted(out_plan,msvc ? "/D%s" : "-D%s",request->definitions[index]); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status != UMI_STATUS_OK) return status; }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (request->debug_information) { status = umi_compiler_plan_add_argument(out_plan,msvc ? "/Zi" : "-g"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status; }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (request->warnings_as_errors) { status = umi_compiler_plan_add_argument(out_plan,msvc ? "/WX" : "-Werror"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status; }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->option_count; ++index) { status = umi_compiler_plan_add_argument(out_plan,request->options[index]); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status != UMI_STATUS_OK) return status; }
    return UMI_STATUS_OK;
}
/*
 * Provide the compiler family name operation used by this module and its client
 * applications.
 */
const char *umi_compiler_family_name(UmiCompilerFamily family) { /* Select the behaviour associated with the requested command or state value. */ switch (family) { case UMI_COMPILER_FAMILY_GCC: return "GCC"; case UMI_COMPILER_FAMILY_CLANG: return "Clang/LLVM"; case UMI_COMPILER_FAMILY_MSVC: return "MSVC"; case UMI_COMPILER_FAMILY_RUSTC: return "Rust"; case UMI_COMPILER_FAMILY_ZIG: return "Zig"; case UMI_COMPILER_FAMILY_ASSEMBLER: return "Assembler"; case UMI_COMPILER_FAMILY_UAI: return "UAI"; case UMI_COMPILER_FAMILY_UMICC: return "Umicc"; default: return "Unknown"; } }
