/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/ansi.h
 *
 * PURPOSE:
 *   Parse common ANSI SGR terminal styling into toolkit-neutral text segments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_ANSI_H
#define UMICOM_TERMINAL_ANSI_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TERMINAL_ANSI_SEGMENT_MAX 256U
#define UMI_TERMINAL_ANSI_DEFAULT_COLOUR 0xFFFFFFFFU

/**
 * Represent the terminal ansi style data shared with callers of this public contract.
 */
typedef struct UmiTerminalAnsiStyle {
    uint32_t foreground;
    uint32_t background;
    int bold;
    int italic;
    int underline;
    int inverse;
} UmiTerminalAnsiStyle;

/**
 * Represent the terminal ansi segment data shared with callers of this public contract.
 */
typedef struct UmiTerminalAnsiSegment {
    size_t source_offset;
    size_t text_offset;
    size_t text_length;
    UmiTerminalAnsiStyle style;
} UmiTerminalAnsiSegment;

/**
 * Represent the terminal ansi document data shared with callers of this public contract.
 */
typedef struct UmiTerminalAnsiDocument {
    char text[UMI_TERMINAL_LINE_CAPACITY];
    UmiTerminalAnsiSegment segments[UMI_TERMINAL_ANSI_SEGMENT_MAX];
    size_t segment_count;
    int truncated;
} UmiTerminalAnsiDocument;

/**
 * Release or reset state held by terminal ansi style so the same storage can be reused
 * safely.
 */
void umi_terminal_ansi_style_reset(UmiTerminalAnsiStyle *style);
/**
 * Read terminal ansi into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_terminal_ansi_parse(const char *source,
                                  UmiTerminalAnsiDocument *out_document);

#ifdef __cplusplus
}
#endif
#endif
