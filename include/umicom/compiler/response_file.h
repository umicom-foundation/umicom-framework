/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/response_file.h
 *
 * PURPOSE:
 *   Publish the public response file contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_RESPONSE_FILE_H
#define UMICOM_COMPILER_RESPONSE_FILE_H
#include "umicom/compiler/plan.h"
/**
 * Write compiler response file in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_compiler_response_file_encode(const UmiCompilerPlan *plan,char *out_text,size_t capacity);
/**
 * Provide the compiler response file argument operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_response_file_argument(const char *path,char *out_argument,size_t capacity);
#endif
