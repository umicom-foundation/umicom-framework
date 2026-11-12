/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/reading_markdown.h
 *
 * PURPOSE:
 *   Project a bounded Markdown reading profile into Framework-owned text and semantic ranges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROWSER_READING_MARKDOWN_H
#define UMICOM_BROWSER_READING_MARKDOWN_H
#include "umicom/browser/reading_document.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BROWSER_MARKDOWN_SOURCE_MAX 65535U
/* This is a deliberately limited reading profile, not a CommonMark parser.
 * Supported: ATX headings with up to three leading spaces; flat minus, plus or star list
 * items; paragraphs; backtick/tilde fences; single-backtick inline code; and
 * single/double-star emphasis with non-whitespace inner boundaries.
 * Inline styles may nest to sixteen levels. Triple-star and multi-backtick
 * inline runs remain literal. Code fences preserve indentation and newlines.
 * Raw HTML, links, images, tables, reference definitions, underscore emphasis,
 * block quotes and nested lists remain text rather than active structures.
 * Backslash escapes apply to ASCII punctuation outside code.
 *
 * Input is an exact readable UTF-8 span with no NUL, DEL or other ASCII control
 * except tab/CR/LF. No trailing NUL is required. The source limit is 65,535 bytes;
 * existing reading text/span capacities also apply. Internal serialization is
 * limited to one MiB and scanning work to 32 * source bytes + 1,024 steps.
 * Exhaustion returns CAPACITY_EXCEEDED without a partial model. Unsupported
 * syntax remains readable; limits do not promise that every bounded input fits.
 *
 * Reuse ReadingDocumentSummary/Text/SpanAt/Destroy for the immutable result.
 * Summary source_bytes counts original Markdown bytes, not internal markup.
 * out_document is cleared on refusal. Inputs and output storage must not alias.
 * No files, links, images, scripts, toolchains or network operations are opened.
 */
UmiStatus UmiBrowserReadingMarkdownCreate(const void *source, size_t bytes,
    UmiBrowserReadingDocument **out_document);
#ifdef __cplusplus
}
#endif
#endif
