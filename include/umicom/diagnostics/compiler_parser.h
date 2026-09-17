/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/compiler_parser.h
 *
 * PURPOSE:
 *   Publish the public compiler parser contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_COMPILER_PARSER_H
#define UMICOM_DIAGNOSTICS_COMPILER_PARSER_H

#include "umicom/diagnostics/parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Compiler text is parsed once, then projected into the bounded build and
 * diagnostic-model records. The path capacity preserves the build API's
 * existing 2 KiB path contract without enlarging UmiDiagnosticSnapshot. */
#define UMI_COMPILER_DIAGNOSTIC_PATH_CAPACITY 2048U

typedef struct UmiCompilerDiagnosticFields {
    char path[UMI_COMPILER_DIAGNOSTIC_PATH_CAPACITY];
    char code[UMI_DIAGNOSTIC_CODE_CAPACITY];
    char message[UMI_DIAGNOSTIC_MESSAGE_CAPACITY];
    size_t line;
    size_t column;
    UmiDiagnosticSeverity severity;
} UmiCompilerDiagnosticFields;

/** Parse one NUL-terminated compiler line. GCC/Clang, MSVC and CMake locations
 * are recognised. ANSI colour/erase sequences are ignored. Positions remain
 * one-based values reported by the producer; zero means unavailable.
 * Input and output are caller-owned. Output changes only on OK. NOT_FOUND
 * means ordinary output; CAPACITY_EXCEEDED means it cannot fit without loss.
 * Locale-specific messages and terminal cursor-control emulation are not parsed. */
UmiStatus UmiCompilerDiagnosticParseText(const char *text,
    UmiCompilerDiagnosticFields *outFields);

/**
 * Provide the compiler diagnostic parser operation used by this module and its client
 * applications.
 */
UmiDiagnosticParser umi_compiler_diagnostic_parser(void);
/**
 * Read compiler diagnostic into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_compiler_diagnostic_parse(const UmiOutputRecord *output,
                                        UmiDiagnosticSnapshot *out_diagnostic,
                                        int *out_matched,
                                        void *user_data);

#ifdef __cplusplus
}
#endif
#endif
