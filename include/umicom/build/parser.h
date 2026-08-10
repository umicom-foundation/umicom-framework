/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/parser.h
 *
 * PURPOSE:
 *   Parse GCC, Clang, MSVC and generic tool output into Framework build diagnostics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_PARSER_H
#define UMICOM_BUILD_PARSER_H

#include "umicom/base/status.h"
#include "umicom/build/diagnostic.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_build_parse_diagnostic_line(const char *line,
                                          UmiBuildDiagnostic *out_diagnostic);
UmiStatus umi_build_parse_output(const char *output,
                                 UmiBuildDiagnosticList *out_list);

#ifdef __cplusplus
}
#endif

#endif
