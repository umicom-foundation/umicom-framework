/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/compiler_parser_internal.h
 * PURPOSE: Share compiler record boundaries between transcript and stream consumers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_COMPILER_PARSER_INTERNAL_H
#define UMICOM_COMPILER_PARSER_INTERNAL_H
/* These are grammar classifications, not a second diagnostic parser. Unknown
 * terminal controls end a pending record instead of acting as indentation. */
typedef enum UmiCompilerLineKind
{
    UMI_COMPILER_LINE_OTHER,
    UMI_COMPILER_LINE_BLANK,
    UMI_COMPILER_LINE_INDENTED,
    UMI_COMPILER_LINE_CMAKE
} UmiCompilerLineKind;
UmiCompilerLineKind UmiCompilerClassifyLine(const char *line);
#endif
