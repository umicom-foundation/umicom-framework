/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/failure_parser.h
 * PURPOSE: Recognise source locations in compiler messages and test assertion output.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DIAGNOSTICS_FAILURE_PARSER_H
#define UMICOM_DIAGNOSTICS_FAILURE_PARSER_H
#include "umicom/diagnostics/compiler_parser.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Parse one line without file I/O. Compiler messages reuse the existing parser.
 * Test assertions accept path:line: message, path:line:column: message, and
 * path(line[,column]): message, with optional CTest verbose "number: " prefix.
 * ANSI colour/erase sequences use the existing compiler cleaning rules.
 * A path must look like a local file, not an arbitrary URL or prose prefix.
 * Positions are positive and one-based; missing column is zero. No source path
 * is inferred from messages such as "check failed at line 28". Output changes
 * only on OK. NOT_FOUND means no supported source location; capacity failure
 * rejects the complete line rather than producing a cropped destination.
 * A recognised location is evidence, not a claim that the file exists or that
 * the diagnostic belongs to the current workspace. Hosts resolve it explicitly. */
UmiStatus UmiTestFailureParseText(const char *text, UmiCompilerDiagnosticFields *outFields);
#ifdef __cplusplus
}
#endif
#endif
