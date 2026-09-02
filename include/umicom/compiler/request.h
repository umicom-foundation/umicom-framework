/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/request.h
 *
 * PURPOSE:
 *   Publish the public request contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_REQUEST_H
#define UMICOM_COMPILER_REQUEST_H
#include "umicom/compiler/target.h"
/**
 * Represent the compiler request data shared with callers of this public contract.
 */
typedef struct UmiCompilerRequest {
    char project_id[UMI_COMPILER_ID_CAPACITY];
    char unit_id[UMI_COMPILER_ID_CAPACITY];
    char source_path[UMI_COMPILER_PATH_CAPACITY];
    char output_path[UMI_COMPILER_PATH_CAPACITY];
    char working_directory[UMI_COMPILER_PATH_CAPACITY];
    char standard[64U];
    UmiCompilerLanguage language;
    UmiCompilerAction action;
    UmiCompilerTarget target;
    char include_directories[UMI_COMPILER_MAX_OPTIONS][UMI_COMPILER_PATH_CAPACITY];
    char definitions[UMI_COMPILER_MAX_OPTIONS][UMI_COMPILER_TEXT_CAPACITY];
    char options[UMI_COMPILER_MAX_OPTIONS][UMI_COMPILER_TEXT_CAPACITY];
    size_t include_directory_count;
    size_t definition_count;
    size_t option_count;
    bool debug_information;
    bool position_independent;
    bool warnings_as_errors;
} UmiCompilerRequest;
/**
 * Initialise compiler request from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_compiler_request_init(UmiCompilerRequest *request,const char *project_id,const char *unit_id,UmiCompilerLanguage language,const char *source_path,const char *output_path);
/**
 * Provide the compiler request add include operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_request_add_include(UmiCompilerRequest *request,const char *directory);
/**
 * Provide the compiler request add definition operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_request_add_definition(UmiCompilerRequest *request,const char *definition);
/**
 * Provide the compiler request add option operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_request_add_option(UmiCompilerRequest *request,const char *option);
/**
 * Check that compiler request satisfies its contract before another service relies on it.
 */
UmiStatus umi_compiler_request_validate(const UmiCompilerRequest *request,char *out_reason,size_t capacity);
#endif
