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

/** Read the first diagnostic record in a NUL-terminated transcript.
 * CMake location headers collect following indented explanation paragraphs.
 * Call stacks, unindented progress text and subsequent CMake headers remain
 * separate records. Other supported formats consume one physical line.
 *
 * The input, fields and byte-count storage must be distinct. With valid
 * arguments, outConsumed advances over the record (or the first ordinary
 * line), including line endings, even when the record exceeds message capacity.
 * It is zero only for empty input. Fields change only on OK. An oversized
 * message returns CAPACITY_EXCEEDED without a cropped diagnostic; the caller
 * can retain the original transcript and count the unrepresented record.
 * Unsupported terminal control sequences are not interpreted as continuations.
 * Invalid arguments leave both outputs unchanged. */
UmiStatus UmiCompilerDiagnosticParseBlock(const char *text,
    UmiCompilerDiagnosticFields *outFields, size_t *outConsumed);

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
