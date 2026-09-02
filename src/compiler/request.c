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
/* Umicom Framework | Compiler requests | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/request.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>
/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,size_t capacity,const char *value)
{ int length; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (destination == NULL || value == NULL || value[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; length = snprintf(destination,capacity,"%s",value); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK; }
/*
 * Initialise compiler request from caller-provided values so later operations receive a
 * known state.
 */
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
/* Provide the add option operation used by this module and its client applications. */
static UmiStatus add_option(char *items,size_t stride,size_t *count,const char *value)
{ int length; char *destination; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (items == NULL || count == NULL || value == NULL || value[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (*count >= UMI_COMPILER_MAX_OPTIONS) return UMI_STATUS_CAPACITY_EXCEEDED; destination = items + (*count * stride); length = snprintf(destination,stride,"%s",value); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (length < 0 || (size_t)length >= stride) return UMI_STATUS_CAPACITY_EXCEEDED; *count += 1U; return UMI_STATUS_OK; }
/*
 * Provide the compiler request add include operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_request_add_include(UmiCompilerRequest *request,const char *directory) { return request == NULL ? UMI_STATUS_INVALID_ARGUMENT : add_option(&request->include_directories[0][0],sizeof(request->include_directories[0]),&request->include_directory_count,directory); }
/*
 * Provide the compiler request add definition operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_request_add_definition(UmiCompilerRequest *request,const char *definition) { return request == NULL ? UMI_STATUS_INVALID_ARGUMENT : add_option(&request->definitions[0][0],sizeof(request->definitions[0]),&request->definition_count,definition); }
/*
 * Provide the compiler request add option operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_request_add_option(UmiCompilerRequest *request,const char *option) { return request == NULL ? UMI_STATUS_INVALID_ARGUMENT : add_option(&request->options[0][0],sizeof(request->options[0]),&request->option_count,option); }
/* Check that compiler request satisfies its contract before another service relies on it. */
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
