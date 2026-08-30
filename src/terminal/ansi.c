/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/ansi.c
 *
 * PURPOSE:
 *   Parse ANSI SGR sequences into bounded styled text segments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/ansi.h"

#include <ctype.h>
#include <string.h>

static const uint32_t COLOURS[16] = {
    0x000000FFU, 0xCD3131FFU, 0x0DBC79FFU, 0xE5E510FFU,
    0x2472C8FFU, 0xBC3FBCFFU, 0x11A8CDFFU, 0xE5E5E5FFU,
    0x666666FFU, 0xF14C4CFFU, 0x23D18BFFU, 0xF5F543FFU,
    0x3B8EEAFFU, 0xD670D6FFU, 0x29B8DBFFU, 0xFFFFFFFFU
};

void umi_terminal_ansi_style_reset(UmiTerminalAnsiStyle *style)
{
    if (style == NULL) return;
    (void)memset(style, 0, sizeof(*style));
    style->foreground = UMI_TERMINAL_ANSI_DEFAULT_COLOUR;
    style->background = UMI_TERMINAL_ANSI_DEFAULT_COLOUR;
}

static void apply_code(UmiTerminalAnsiStyle *style, unsigned code)
{
    if (code == 0U) umi_terminal_ansi_style_reset(style);
    else if (code == 1U) style->bold = 1;
    else if (code == 3U) style->italic = 1;
    else if (code == 4U) style->underline = 1;
    else if (code == 7U) style->inverse = 1;
    else if (code == 22U) style->bold = 0;
    else if (code == 23U) style->italic = 0;
    else if (code == 24U) style->underline = 0;
    else if (code == 27U) style->inverse = 0;
    else if (code >= 30U && code <= 37U) style->foreground = COLOURS[code - 30U];
    else if (code >= 90U && code <= 97U) style->foreground = COLOURS[code - 90U + 8U];
    else if (code >= 40U && code <= 47U) style->background = COLOURS[code - 40U];
    else if (code >= 100U && code <= 107U) style->background = COLOURS[code - 100U + 8U];
    else if (code == 39U) style->foreground = UMI_TERMINAL_ANSI_DEFAULT_COLOUR;
    else if (code == 49U) style->background = UMI_TERMINAL_ANSI_DEFAULT_COLOUR;
}

static void append_segment(UmiTerminalAnsiDocument *document,
                           size_t source_offset,
                           size_t text_offset,
                           size_t length,
                           const UmiTerminalAnsiStyle *style)
{
    UmiTerminalAnsiSegment *segment;
    if (length == 0U) return;
    if (document->segment_count >= UMI_TERMINAL_ANSI_SEGMENT_MAX) {
        document->truncated = 1;
        return;
    }
    segment = &document->segments[document->segment_count++];
    segment->source_offset = source_offset;
    segment->text_offset = text_offset;
    segment->text_length = length;
    segment->style = *style;
}

UmiStatus umi_terminal_ansi_parse(const char *source,
                                  UmiTerminalAnsiDocument *out_document)
{
    UmiTerminalAnsiStyle style;
    size_t source_index = 0U;
    size_t output_index = 0U;
    size_t segment_source = 0U;
    size_t segment_output = 0U;
    if (source == NULL || out_document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_document, 0, sizeof(*out_document));
    umi_terminal_ansi_style_reset(&style);
    while (source[source_index] != '\0') {
        if ((unsigned char)source[source_index] == 0x1BU &&
            source[source_index + 1U] == '[') {
            size_t cursor = source_index + 2U;
            unsigned code = 0U;
            int have_code = 0;
            append_segment(out_document, segment_source, segment_output,
                           output_index - segment_output, &style);
            while (source[cursor] != '\0' && source[cursor] != 'm') {
                if (isdigit((unsigned char)source[cursor])) {
                    code = code * 10U + (unsigned)(source[cursor] - '0');
                    have_code = 1;
                } else if (source[cursor] == ';') {
                    apply_code(&style, have_code ? code : 0U);
                    code = 0U;
                    have_code = 0;
                } else {
                    break;
                }
                cursor += 1U;
            }
            if (source[cursor] == 'm') {
                apply_code(&style, have_code ? code : 0U);
                source_index = cursor + 1U;
                segment_source = source_index;
                segment_output = output_index;
                continue;
            }
        }
        if (output_index + 1U >= sizeof(out_document->text)) {
            out_document->truncated = 1;
            break;
        }
        out_document->text[output_index++] = source[source_index++];
    }
    out_document->text[output_index] = '\0';
    append_segment(out_document, segment_source, segment_output,
                   output_index - segment_output, &style);
    return UMI_STATUS_OK;
}
