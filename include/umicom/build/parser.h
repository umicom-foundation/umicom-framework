/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/parser.h
 *
 * PURPOSE:
 *   Parse GCC, Clang, MSVC and generic tool output into Framework build diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_PARSER_H
#define UMICOM_BUILD_PARSER_H

#include "umicom/base/status.h"
#include "umicom/build/diagnostic.h"
#include "umicom/diagnostics/compiler_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Copy a shared compiler record into the bounded build representation. All
 * fields must be terminated. Failure leaves out unchanged and returns
 * CAPACITY_EXCEEDED rather than cropping a path or diagnostic explanation.
 */
UmiStatus UmiBuildDiagnosticFromCompilerFields(const UmiCompilerDiagnosticFields *fields,
    UmiBuildDiagnostic *out);

UmiStatus umi_build_parse_diagnostic_line(const char *line,
                                          UmiBuildDiagnostic *out_diagnostic);
/* Parse a complete transcript using the shared compiler grammar. CMake
 * explanations join their source header; call stacks remain in the raw log.
 * Oversized records increment dropped instead of exposing cropped messages. */
UmiStatus umi_build_parse_output(const char *output,
                                 UmiBuildDiagnosticList *out_list);

#ifdef __cplusplus
}
#endif

#endif
