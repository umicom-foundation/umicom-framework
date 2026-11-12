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
#include "umicom/compiler/provider.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "catalogue_validation.h"
/* Provide the add formatted operation used by this module and its client applications. */
static UmiStatus add_formatted(UmiCompilerPlan *plan,const char *format,const char *value)
{ char buffer[UMI_COMPILER_TEXT_CAPACITY]; int length = snprintf(buffer,sizeof(buffer),format,value); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (length < 0 || (size_t)length >= sizeof(buffer)) return UMI_STATUS_CAPACITY_EXCEEDED; return umi_compiler_plan_add_argument(plan,buffer); }
/*
 * Initialise compiler provider from caller-provided values so later operations receive a
 * known state.
 */
/* Shared bounded record admission and staged publication protect direct compiler clients and borrowed initialization inputs; existing classification and compatibility policy remain. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_provider_init(UmiCompilerProvider *provider,const UmiCompilerProviderDescriptor *descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL || descriptor == NULL || descriptor->struct_size < sizeof(*descriptor) || descriptor->api_version != UMI_COMPILER_API_VERSION || descriptor->provider_id[0] == '\0' || descriptor->display_name[0] == '\0' || descriptor->family == UMI_COMPILER_FAMILY_UNKNOWN || descriptor->language_mask == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(provider,0,sizeof(*provider)); provider->descriptor = *descriptor; provider->revision = 1U; return UMI_STATUS_OK;
}
#endif
UmiStatus umi_compiler_provider_init(UmiCompilerProvider *provider,const UmiCompilerProviderDescriptor *descriptor)
{
    if (provider == NULL || descriptor == NULL || descriptor->struct_size < sizeof(*descriptor) ||
        descriptor->api_version != UMI_COMPILER_API_VERSION || descriptor->provider_id[0] == '\0' ||
        descriptor->display_name[0] == '\0' || descriptor->family == UMI_COMPILER_FAMILY_UNKNOWN ||
        descriptor->language_mask == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Descriptors can be borrowed from the existing provider. Copy first and
     * reuse catalogue text admission instead of inventing a second boundary. */
    UmiCompilerProvider candidate = {0};
    candidate.descriptor = *descriptor;
    if (!UmiCompilerCatalogueProvider(&candidate)) return UMI_STATUS_INVALID_ARGUMENT;
    candidate.revision = 1U;
    *provider = candidate;
    return UMI_STATUS_OK;
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
/* Shared bounded record admission and staged publication protect direct compiler clients and borrowed initialization inputs; existing classification and compatibility policy remain. The previous implementation is retained for review. */
#if 0
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
#endif
/* Stage the complete plan to preserve caller output on every later argument failure; a plain value-construction wrapper cannot rebase this record's internal pointers. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_provider_plan(const UmiCompilerProvider *provider,const UmiCompilerRequest *request,UmiCompilerPlan *out_plan)
{
    UmiStatus status; char reason[UMI_COMPILER_TEXT_CAPACITY]; size_t index; bool msvc;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UmiCompilerCatalogueProvider(provider) || request == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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
#endif
/* The original provider-specific option mapping now writes only to a private candidate. */
/* This query describes planner mappings, independently of installation state.
 * Language masks remain necessary but do not imply that every action is mapped. */
bool umi_compiler_provider_can_plan_action(const UmiCompilerProvider *provider,
                                          UmiCompilerLanguage language,
                                          UmiCompilerAction action)
{
    if (!UmiCompilerCatalogueProvider(provider) ||
        !umi_compiler_provider_supports(provider, language) ||
        provider->descriptor.family <= UMI_COMPILER_FAMILY_UNKNOWN ||
        provider->descriptor.family > UMI_COMPILER_FAMILY_UMICC) return false;
    switch (action) {
    case UMI_COMPILER_ACTION_COMPILE:
        return true; /* Retain the established provider-specific compile mapping. */
    case UMI_COMPILER_ACTION_ASSEMBLE:
        return language == UMI_COMPILER_LANGUAGE_ASSEMBLY;
    case UMI_COMPILER_ACTION_CHECK:
        return (language == UMI_COMPILER_LANGUAGE_C ||
                language == UMI_COMPILER_LANGUAGE_CPP) &&
            (provider->descriptor.family == UMI_COMPILER_FAMILY_GCC ||
             provider->descriptor.family == UMI_COMPILER_FAMILY_CLANG ||
             provider->descriptor.family == UMI_COMPILER_FAMILY_MSVC);
    default:
        /* Linking and archiving need dedicated adapters; a compile command
         * carrying a different metadata label is not an implementation. */
        return false;
    }
}

/* Action admission and explicit C/C++ syntax-check planning replace accidental compile plans for other action labels; existing compile/assembly option mappings are retained. The previous builder is The previous implementation is retained for review. */
#if 0
static UmiStatus UmiCompilerProviderBuildPlan(const UmiCompilerProvider *provider,const UmiCompilerRequest *request,UmiCompilerPlan *out_plan)
{
    UmiStatus status; char reason[UMI_COMPILER_TEXT_CAPACITY]; size_t index; bool msvc;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UmiCompilerCatalogueProvider(provider) || request == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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
#endif
/* The shared planner now maps or refuses position-independent intent before publishing a plan. The previous implementation is retained for review. */
#if 0
static UmiStatus UmiCompilerProviderBuildPlan(const UmiCompilerProvider *provider,const UmiCompilerRequest *request,UmiCompilerPlan *out_plan)
{
    UmiStatus status; char reason[UMI_COMPILER_TEXT_CAPACITY]; size_t index; bool msvc; bool checking;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UmiCompilerCatalogueProvider(provider) || request == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_compiler_request_validate(request,reason,sizeof(reason)); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_compiler_provider_can_plan_action(provider,request->language,request->action)) return UMI_STATUS_NOT_IMPLEMENTED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!provider->descriptor.available) return UMI_STATUS_UNAVAILABLE;
    umi_compiler_plan_init(out_plan);
    (void)snprintf(out_plan->provider_id,sizeof(out_plan->provider_id),"%s",provider->descriptor.provider_id);
    (void)snprintf(out_plan->executable,sizeof(out_plan->executable),"%s",provider->descriptor.executable);
    (void)snprintf(out_plan->working_directory,sizeof(out_plan->working_directory),"%s",request->working_directory);
    out_plan->language = request->language; out_plan->action = request->action; msvc = provider->descriptor.family == UMI_COMPILER_FAMILY_MSVC;
    checking = request->action == UMI_COMPILER_ACTION_CHECK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (checking) {
        /* Use each C/C++ driver's documented syntax-only stage. Do not generate
         * object-output or debug-artifact switches for this action. Raw caller
         * options are still explicit inputs and require separate policy review. */
        status = umi_compiler_plan_add_argument(out_plan, msvc ? "/nologo" : "-fsyntax-only");
        if (status == UMI_STATUS_OK && msvc)
            status = umi_compiler_plan_add_argument(out_plan, "/Zs");
        if (status == UMI_STATUS_OK)
            status = add_formatted(out_plan, msvc ? "/std:%s" : "-std=%s", request->standard);
        if (status == UMI_STATUS_OK && provider->descriptor.family == UMI_COMPILER_FAMILY_CLANG)
            status = add_formatted(out_plan, "--target=%s", request->target.triple);
        if (status == UMI_STATUS_OK)
            status = umi_compiler_plan_add_argument(out_plan, request->source_path);
    } else if (provider->descriptor.family == UMI_COMPILER_FAMILY_RUSTC) {
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
    if (request->debug_information && !checking) { status = umi_compiler_plan_add_argument(out_plan,msvc ? "/Zi" : "-g"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status; }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (request->warnings_as_errors) { status = umi_compiler_plan_add_argument(out_plan,msvc ? "/WX" : "-Werror"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status; }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->option_count; ++index) { status = umi_compiler_plan_add_argument(out_plan,request->options[index]); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status != UMI_STATUS_OK) return status; }
    return UMI_STATUS_OK;
}
#endif
static UmiStatus UmiCompilerProviderBuildPlan(const UmiCompilerProvider *provider,const UmiCompilerRequest *request,UmiCompilerPlan *out_plan)
{
    UmiStatus status; char reason[UMI_COMPILER_TEXT_CAPACITY]; size_t index; bool msvc; bool checking;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UmiCompilerCatalogueProvider(provider) || request == NULL || out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_compiler_request_validate(request,reason,sizeof(reason)); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_compiler_provider_can_plan_action(provider,request->language,request->action)) return UMI_STATUS_NOT_IMPLEMENTED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!provider->descriptor.available) return UMI_STATUS_UNAVAILABLE;
    /* Code-generation intent must be mapped explicitly. A driver family or
     * action without a reviewed mapping must not silently ignore this request.
     * Target-specific support still needs qualification with the actual driver. */
    if (request->position_independent &&
        (request->action != UMI_COMPILER_ACTION_COMPILE ||
         (request->language != UMI_COMPILER_LANGUAGE_C && request->language != UMI_COMPILER_LANGUAGE_CPP) ||
         (provider->descriptor.family != UMI_COMPILER_FAMILY_GCC &&
          provider->descriptor.family != UMI_COMPILER_FAMILY_CLANG)))
        return UMI_STATUS_NOT_IMPLEMENTED;
    umi_compiler_plan_init(out_plan);
    (void)snprintf(out_plan->provider_id,sizeof(out_plan->provider_id),"%s",provider->descriptor.provider_id);
    (void)snprintf(out_plan->executable,sizeof(out_plan->executable),"%s",provider->descriptor.executable);
    (void)snprintf(out_plan->working_directory,sizeof(out_plan->working_directory),"%s",request->working_directory);
    out_plan->language = request->language; out_plan->action = request->action; msvc = provider->descriptor.family == UMI_COMPILER_FAMILY_MSVC;
    checking = request->action == UMI_COMPILER_ACTION_CHECK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (checking) {
        /* Use each C/C++ driver's documented syntax-only stage. Do not generate
         * object-output or debug-artifact switches for this action. Raw caller
         * options are still explicit inputs and require separate policy review. */
        status = umi_compiler_plan_add_argument(out_plan, msvc ? "/nologo" : "-fsyntax-only");
        if (status == UMI_STATUS_OK && msvc)
            status = umi_compiler_plan_add_argument(out_plan, "/Zs");
        if (status == UMI_STATUS_OK)
            status = add_formatted(out_plan, msvc ? "/std:%s" : "-std=%s", request->standard);
        if (status == UMI_STATUS_OK && provider->descriptor.family == UMI_COMPILER_FAMILY_CLANG)
            status = add_formatted(out_plan, "--target=%s", request->target.triple);
        if (status == UMI_STATUS_OK)
            status = umi_compiler_plan_add_argument(out_plan, request->source_path);
    } else if (provider->descriptor.family == UMI_COMPILER_FAMILY_RUSTC) {
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
    if (request->debug_information && !checking) { status = umi_compiler_plan_add_argument(out_plan,msvc ? "/Zi" : "-g"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status; }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (request->warnings_as_errors) { status = umi_compiler_plan_add_argument(out_plan,msvc ? "/WX" : "-Werror"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status; }
    /* Visit each bounded item once so every record receives the same rule. */
    if (request->position_independent) {
        status = umi_compiler_plan_add_argument(out_plan,"-fPIC");
        if (status != UMI_STATUS_OK) return status;
    }
    /* Explicit raw options follow structured intent; callers reviewing such
     * options remain responsible for contradictory driver switches. */
    for (index = 0U; index < request->option_count; ++index) { status = umi_compiler_plan_add_argument(out_plan,request->options[index]); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (status != UMI_STATUS_OK) return status; }
    return UMI_STATUS_OK;
}

UmiStatus umi_compiler_provider_plan(const UmiCompilerProvider *provider,const UmiCompilerRequest *request,UmiCompilerPlan *out_plan)
{
    if (!UmiCompilerCatalogueProvider(provider) || request == NULL || out_plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Plans contain a sizeable inline argument matrix and pointers back into
     * it. Build off-stack, publish once, then rebase every borrowed pointer. */
    UmiCompilerPlan *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiCompilerProviderBuildPlan(provider,request,candidate);
    if (status == UMI_STATUS_OK && !umi_compiler_plan_storage_valid(candidate)) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        (void)memcpy(out_plan,candidate,sizeof(*out_plan));
        (void)umi_compiler_plan_arguments(out_plan);
    }
    free(candidate);
    return status;
}
/*
 * Provide the compiler family name operation used by this module and its client
 * applications.
 */
const char *umi_compiler_family_name(UmiCompilerFamily family) { /* Select the behaviour associated with the requested command or state value. */ switch (family) { case UMI_COMPILER_FAMILY_GCC: return "GCC"; case UMI_COMPILER_FAMILY_CLANG: return "Clang/LLVM"; case UMI_COMPILER_FAMILY_MSVC: return "MSVC"; case UMI_COMPILER_FAMILY_RUSTC: return "Rust"; case UMI_COMPILER_FAMILY_ZIG: return "Zig"; case UMI_COMPILER_FAMILY_ASSEMBLER: return "Assembler"; case UMI_COMPILER_FAMILY_UAI: return "UAI"; case UMI_COMPILER_FAMILY_UMICC: return "Umicc"; default: return "Unknown"; } }
