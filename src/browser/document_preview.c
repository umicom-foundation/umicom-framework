/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/document_preview.c
 *
 * PURPOSE:
 *   Build a safe readable preview from HTML for frontends that do not have a
 *   native browser renderer available.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/browser/document_preview.h"

#include <ctype.h>
#include <string.h>

/* Append one character while preserving room for the terminating null byte. */
static void preview_append_character(
    UmiBrowserDocumentPreview *preview,
    char character)
{
    if (preview->text_bytes + 1U >= sizeof(preview->text)) {
        preview->truncated = 1;
        return;
    }
    preview->text[preview->text_bytes++] = character;
    preview->text[preview->text_bytes] = '\0';
}

/* Separate block elements without producing long runs of empty lines. */
static void preview_append_break(UmiBrowserDocumentPreview *preview)
{
    while (preview->text_bytes > 0U &&
           preview->text[preview->text_bytes - 1U] == ' ') {
        --preview->text_bytes;
    }
    preview->text[preview->text_bytes] = '\0';
    if (preview->text_bytes > 0U &&
        preview->text[preview->text_bytes - 1U] != '\n') {
        preview_append_character(preview, '\n');
    }
}

/* Compare an ASCII tag name without depending on locale-specific case rules. */
static int preview_tag_is(
    const char *tag,
    size_t tag_length,
    const char *expected)
{
    size_t index;
    size_t expected_length = strlen(expected);
    if (tag_length != expected_length) return 0;
    for (index = 0U; index < tag_length; ++index) {
        if (tolower((unsigned char)tag[index]) !=
            tolower((unsigned char)expected[index])) {
            return 0;
        }
    }
    return 1;
}

/* Identify elements whose visible content should start on a new line. */
static int preview_tag_is_block(const char *tag, size_t tag_length)
{
    static const char *const BLOCK_TAGS[] = {
        "br", "p", "div", "section", "article", "header", "footer",
        "main", "nav", "aside", "li", "tr", "h1", "h2", "h3", "h4",
        "h5", "h6", "pre", "blockquote"
    };
    size_t index;
    for (index = 0U;
         index < sizeof(BLOCK_TAGS) / sizeof(BLOCK_TAGS[0]);
         ++index) {
        if (preview_tag_is(tag, tag_length, BLOCK_TAGS[index])) return 1;
    }
    return 0;
}

/* Find a short marker without reading beyond caller-provided HTML bounds. */
static const char *preview_find_sequence(
    const char *text,
    size_t available,
    const char *sequence)
{
    size_t sequence_length = strlen(sequence);
    size_t index;
    if (sequence_length == 0U || available < sequence_length) return NULL;
    for (index = 0U; index + sequence_length <= available; ++index) {
        if (memcmp(&text[index], sequence, sequence_length) == 0) {
            return &text[index];
        }
    }
    return NULL;
}

/* Decode the small entity set needed by ordinary documentation prose. */
static size_t preview_decode_entity(
    const char *text,
    size_t remaining,
    char *out_character)
{
    static const struct {
        const char *entity;
        char value;
    } ENTITIES[] = {
        {"&amp;", '&'}, {"&lt;", '<'}, {"&gt;", '>'},
        {"&quot;", '"'}, {"&apos;", '\''}, {"&nbsp;", ' '}
    };
    size_t index;
    for (index = 0U; index < sizeof(ENTITIES) / sizeof(ENTITIES[0]); ++index) {
        size_t length = strlen(ENTITIES[index].entity);
        if (remaining >= length &&
            strncmp(text, ENTITIES[index].entity, length) == 0) {
            *out_character = ENTITIES[index].value;
            return length;
        }
    }
    return 0U;
}

/* Copy a bounded title and trim whitespace from both ends. */
static void preview_store_title(
    UmiBrowserDocumentPreview *preview,
    const char *start,
    size_t length)
{
    size_t copy_length;
    while (length > 0U && isspace((unsigned char)*start)) {
        ++start;
        --length;
    }
    while (length > 0U && isspace((unsigned char)start[length - 1U])) {
        --length;
    }
    copy_length = length < sizeof(preview->title) - 1U
        ? length
        : sizeof(preview->title) - 1U;
    if (copy_length > 0U) {
        (void)memcpy(preview->title, start, copy_length);
    }
    preview->title[copy_length] = '\0';
}

/* Reset every field so a reused preview never exposes earlier document text. */
void umi_browser_document_preview_init(UmiBrowserDocumentPreview *preview)
{
    if (preview == NULL) return;
    (void)memset(preview, 0, sizeof(*preview));
    preview->structure_size = (uint32_t)sizeof(*preview);
    preview->api_version = 1U;
}

/* Walk bounded HTML once and copy only visible, readable document content. */
UmiStatus umi_browser_document_preview_from_html(
    const char *html,
    size_t html_size,
    UmiBrowserDocumentPreview *out_preview)
{
    size_t index = 0U;
    int suppressed_depth = 0;
    int pending_space = 0;

    if (html == NULL || out_preview == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_browser_document_preview_init(out_preview);
    out_preview->source_bytes = html_size;

    while (index < html_size && html[index] != '\0') {
        if (html[index] == '<') {
            size_t cursor = index + 1U;
            size_t tag_start;
            size_t tag_length;
            int closing = 0;

            /* Skip comments as a single unit so angle brackets inside a
             * comment cannot be mistaken for visible document content. */
            if (cursor + 2U < html_size &&
                strncmp(&html[cursor], "!--", 3U) == 0) {
                const char *end = preview_find_sequence(
                    &html[cursor + 3U],
                    html_size - cursor - 3U,
                    "-->");
                index = end != NULL
                    ? (size_t)(end - html) + 3U
                    : html_size;
                continue;
            }
            if (cursor < html_size && html[cursor] == '/') {
                closing = 1;
                ++cursor;
            }
            while (cursor < html_size &&
                   isspace((unsigned char)html[cursor])) {
                ++cursor;
            }
            tag_start = cursor;
            while (cursor < html_size &&
                   (isalnum((unsigned char)html[cursor]) ||
                    html[cursor] == '-' || html[cursor] == '_')) {
                ++cursor;
            }
            tag_length = cursor - tag_start;

            if (preview_tag_is(&html[tag_start], tag_length, "script") ||
                preview_tag_is(&html[tag_start], tag_length, "style")) {
                suppressed_depth += closing ? -1 : 1;
                if (suppressed_depth < 0) suppressed_depth = 0;
            }
            if (suppressed_depth == 0 &&
                preview_tag_is_block(&html[tag_start], tag_length)) {
                preview_append_break(out_preview);
                pending_space = 0;
            }
            if (!closing &&
                preview_tag_is(&html[tag_start], tag_length, "title")) {
                const char *title_start;
                const char *title_end;
                while (cursor < html_size && html[cursor] != '>') ++cursor;
                title_start = cursor < html_size ? &html[cursor + 1U] : NULL;
                title_end = title_start != NULL
                    ? preview_find_sequence(
                        title_start,
                        html_size - (size_t)(title_start - html),
                        "</title")
                    : NULL;
                if (title_start != NULL && title_end != NULL) {
                    preview_store_title(
                        out_preview,
                        title_start,
                        (size_t)(title_end - title_start));
                }
            }
            while (cursor < html_size && html[cursor] != '>') ++cursor;
            index = cursor < html_size ? cursor + 1U : html_size;
            continue;
        }

        if (suppressed_depth == 0) {
            char character = html[index];
            size_t consumed = 1U;
            if (character == '&') {
                size_t decoded = preview_decode_entity(
                    &html[index], html_size - index, &character);
                if (decoded > 0U) consumed = decoded;
            }
            if (isspace((unsigned char)character)) {
                pending_space = out_preview->text_bytes > 0U &&
                    out_preview->text[out_preview->text_bytes - 1U] != '\n';
            } else {
                if (pending_space) preview_append_character(out_preview, ' ');
                preview_append_character(out_preview, character);
                pending_space = 0;
            }
            index += consumed;
        } else {
            ++index;
        }
    }

    while (out_preview->text_bytes > 0U &&
           isspace((unsigned char)
               out_preview->text[out_preview->text_bytes - 1U])) {
        --out_preview->text_bytes;
    }
    out_preview->text[out_preview->text_bytes] = '\0';
    return UMI_STATUS_OK;
}
