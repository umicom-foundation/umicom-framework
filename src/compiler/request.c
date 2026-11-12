/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/request.c
 *
 * PURPOSE:
 *   Implement the request behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/request.h"
#include "../base/value_construction_internal.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>
#include "catalogue_validation.h"

/* Storage validity permits an initialized draft. Required execution metadata is
 * checked separately by request_validate. Only active option slots are read. */
bool umi_compiler_request_storage_valid(const UmiCompilerRequest *request)
{
    if (request == NULL || request->include_directory_count > UMI_COMPILER_MAX_OPTIONS ||
        request->definition_count > UMI_COMPILER_MAX_OPTIONS || request->option_count > UMI_COMPILER_MAX_OPTIONS)
        return false;
    if (!UmiCompilerCatalogueText(request->project_id,sizeof(request->project_id),false) ||
        !UmiCompilerCatalogueText(request->unit_id,sizeof(request->unit_id),false) ||
        !UmiCompilerCatalogueText(request->source_path,sizeof(request->source_path),false) ||
        !UmiCompilerCatalogueText(request->output_path,sizeof(request->output_path),false) ||
        !UmiCompilerCatalogueText(request->working_directory,sizeof(request->working_directory),false) ||
        !UmiCompilerCatalogueText(request->standard,sizeof(request->standard),false) ||
        !UmiCompilerCatalogueTarget(&request->target)) return false;
    for (size_t index = 0U; index < request->include_directory_count; ++index)
        if (!UmiCompilerCatalogueText(request->include_directories[index],sizeof(request->include_directories[index]),true)) return false;
    for (size_t index = 0U; index < request->definition_count; ++index)
        if (!UmiCompilerCatalogueText(request->definitions[index],sizeof(request->definitions[index]),true)) return false;
    for (size_t index = 0U; index < request->option_count; ++index)
        if (!UmiCompilerCatalogueText(request->options[index],sizeof(request->options[index]),true)) return false;
    return true;
}

/* Provide the copy text operation used by this module and its client applications. */
/* Bounded text admission replaces formatting that could truncate a temporary field; overlap-safe copying retains borrowed-input behavior. The previous implementation is retained for review. */
#if 0
static UmiStatus copy_text(char *destination,size_t capacity,const char *value)
{ int length; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (destination == NULL || value == NULL || value[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; length = snprintf(destination,capacity,"%s",value); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK; }
#endif
static UmiStatus copy_text(char *destination,size_t capacity,const char *value)
{
    if (destination == NULL || value == NULL || value[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiCompilerCatalogueText(value,capacity,true)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memmove(destination,value,strlen(value) + 1U);
    return UMI_STATUS_OK;
}
/*
 * Initialise compiler request from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_compiler_request_init(UmiCompilerRequest *request,const char *project_id,const char *unit_id,UmiCompilerLanguage language,const char *source_path,const char *output_path)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || language <= UMI_COMPILER_LANGUAGE_UNKNOWN || language > UMI_COMPILER_LANGUAGE_UAI) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(request,0,sizeof(*request));
    status = copy_text(request->project_id,sizeof(request->project_id),project_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    status = copy_text(request->unit_id,sizeof(request->unit_id),unit_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    status = copy_text(request->source_path,sizeof(request->source_path),source_path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    status = copy_text(request->output_path,sizeof(request->output_path),output_path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    request->language = language; request->action = language == UMI_COMPILER_LANGUAGE_ASSEMBLY ? UMI_COMPILER_ACTION_ASSEMBLE : UMI_COMPILER_ACTION_COMPILE;
    (void)snprintf(request->standard,sizeof(request->standard),"%s",umi_compiler_language_default_standard(language));
    return umi_compiler_target_host(&request->target);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_compiler_request_init_build_value(UmiCompilerRequest *request,const char *project_id,const char *unit_id,UmiCompilerLanguage language,const char *source_path,const char *output_path)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || language <= UMI_COMPILER_LANGUAGE_UNKNOWN || language > UMI_COMPILER_LANGUAGE_UAI) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(request,0,sizeof(*request));
    status = copy_text(request->project_id,sizeof(request->project_id),project_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    status = copy_text(request->unit_id,sizeof(request->unit_id),unit_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    status = copy_text(request->source_path,sizeof(request->source_path),source_path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    status = copy_text(request->output_path,sizeof(request->output_path),output_path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    request->language = language; request->action = language == UMI_COMPILER_LANGUAGE_ASSEMBLY ? UMI_COMPILER_ACTION_ASSEMBLE : UMI_COMPILER_ACTION_COMPILE;
    (void)snprintf(request->standard,sizeof(request->standard),"%s",umi_compiler_language_default_standard(language));
    return umi_compiler_target_host(&request->target);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_compiler_request_init, UmiCompilerRequest, request,
    (UmiCompilerRequest *request, const char *project_id, const char *unit_id, UmiCompilerLanguage language, const char *source_path, const char *output_path),
    umi_compiler_request_init_build_value(candidate, project_id, unit_id, language, source_path, output_path))

/* Provide the add option operation used by this module and its client applications. */
/* Validate the complete option before changing even unused owner storage; count capacity remains explicit. The previous implementation is retained for review. */
#if 0
static UmiStatus add_option(char *items,size_t stride,size_t *count,const char *value)
{ int length; char *destination; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (items == NULL || count == NULL || value == NULL || value[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (*count >= UMI_COMPILER_MAX_OPTIONS) return UMI_STATUS_CAPACITY_EXCEEDED; destination = items + (*count * stride); length = snprintf(destination,stride,"%s",value); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (length < 0 || (size_t)length >= stride) return UMI_STATUS_CAPACITY_EXCEEDED; *count += 1U; return UMI_STATUS_OK; }
#endif
static UmiStatus add_option(char *items,size_t stride,size_t *count,const char *value)
{
    if (items == NULL || count == NULL || value == NULL || value[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (*count > UMI_COMPILER_MAX_OPTIONS) return UMI_STATUS_INVALID_STATE;
    if (*count == UMI_COMPILER_MAX_OPTIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!UmiCompilerCatalogueText(value,stride,true)) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* All fallible admission precedes publication, including when value points
     * into a previous slot or the destination slot itself. */
    (void)memmove(items + (*count * stride),value,strlen(value) + 1U);
    *count += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the compiler request add include operation used by this module and its client
 * applications.
 */
/* Validate the complete request storage before incremental composition so a corrupted unrelated row is not silently retained. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_request_add_include(UmiCompilerRequest *request,const char *directory) { return request == NULL ? UMI_STATUS_INVALID_ARGUMENT : add_option(&request->include_directories[0][0],sizeof(request->include_directories[0]),&request->include_directory_count,directory); }
#endif
UmiStatus umi_compiler_request_add_include(UmiCompilerRequest *request,const char *directory)
{
    if (request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_request_storage_valid(request)) return UMI_STATUS_INVALID_STATE;
    return add_option(&request->include_directories[0][0],sizeof(request->include_directories[0]),&request->include_directory_count,directory);
}
/*
 * Provide the compiler request add definition operation used by this module and its client
 * applications.
 */
/* Validate the complete request storage before incremental composition so a corrupted unrelated row is not silently retained. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_request_add_definition(UmiCompilerRequest *request,const char *definition) { return request == NULL ? UMI_STATUS_INVALID_ARGUMENT : add_option(&request->definitions[0][0],sizeof(request->definitions[0]),&request->definition_count,definition); }
#endif
UmiStatus umi_compiler_request_add_definition(UmiCompilerRequest *request,const char *definition)
{
    if (request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_request_storage_valid(request)) return UMI_STATUS_INVALID_STATE;
    return add_option(&request->definitions[0][0],sizeof(request->definitions[0]),&request->definition_count,definition);
}
/*
 * Provide the compiler request add option operation used by this module and its client
 * applications.
 */
/* Validate the complete request storage before incremental composition so a corrupted unrelated row is not silently retained. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_request_add_option(UmiCompilerRequest *request,const char *option) { return request == NULL ? UMI_STATUS_INVALID_ARGUMENT : add_option(&request->options[0][0],sizeof(request->options[0]),&request->option_count,option); }
#endif
UmiStatus umi_compiler_request_add_option(UmiCompilerRequest *request,const char *option)
{
    if (request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_request_storage_valid(request)) return UMI_STATUS_INVALID_STATE;
    return add_option(&request->options[0][0],sizeof(request->options[0]),&request->option_count,option);
}
/* Check that compiler request satisfies its contract before another service relies on it. */
/* Check counts and bounded text before execution metadata; diagnostics retain their caller-owned buffer and capacity result. The previous implementation is retained for review. */
#if 0
UmiStatus umi_compiler_request_validate(const UmiCompilerRequest *request,char *out_reason,size_t capacity)
{
    int length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (request->project_id[0] == '\0' || request->unit_id[0] == '\0' || request->source_path[0] == '\0' || request->output_path[0] == '\0') { length = snprintf(out_reason,capacity,"Project, unit, source and output are required"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (request->language == UMI_COMPILER_LANGUAGE_UNKNOWN) { length = snprintf(out_reason,capacity,"Compiler language is unknown"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; }
    length = snprintf(out_reason,capacity,"Compiler request is valid"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
#endif
UmiStatus umi_compiler_request_validate(const UmiCompilerRequest *request,char *out_reason,size_t capacity)
{
    if (request == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    const char *reason = "Compiler request is valid";
    UmiStatus status = UMI_STATUS_OK;
    if (!umi_compiler_request_storage_valid(request)) {
        reason = "Compiler request storage is malformed"; status = UMI_STATUS_INVALID_STATE;
    } else if (request->project_id[0] == '\0' || request->unit_id[0] == '\0' ||
        request->source_path[0] == '\0' || request->output_path[0] == '\0') {
        reason = "Project, unit, source and output are required"; status = UMI_STATUS_INVALID_STATE;
    } else if (request->language <= UMI_COMPILER_LANGUAGE_UNKNOWN || request->language > UMI_COMPILER_LANGUAGE_UAI) {
        reason = "Compiler language is unknown"; status = UMI_STATUS_INVALID_STATE;
    } else if (request->action < UMI_COMPILER_ACTION_COMPILE || request->action > UMI_COMPILER_ACTION_CHECK) {
        reason = "Compiler action is unknown"; status = UMI_STATUS_INVALID_STATE;
    }
    const int length = snprintf(out_reason,capacity,"%s",reason);
    return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : status;
}
