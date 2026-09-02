/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/link.c
 *
 * PURPOSE:
 *   Detect common actionable links in terminal output with conservative,
 *   platform-neutral parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/link.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Provide the terminal delimiter operation used by this module and its client
 * applications.
 */
static int terminal_delimiter(char value)
{
    return value == '\0' || isspace((unsigned char)value) || value == '"' ||
           value == '\'' || value == '<' || value == '>' || value == ')' ||
           value == ']' || value == '}';
}

/* Provide the add link operation used by this module and its client applications. */
static UmiStatus add_link(UmiTerminalLinkResult *result,
                          UmiTerminalLinkKind kind,
                          const char *source,
                          size_t offset,
                          size_t length,
                          uint32_t line,
                          uint32_t column)
{
    UmiTerminalLink *link;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (result->count >= UMI_TERMINAL_LINK_MAX) {
        result->truncated = 1;
        return UMI_STATUS_OK;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > UMI_TERMINAL_PATH_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    link = &result->links[result->count++];
    (void)memset(link, 0, sizeof(*link));
    link->kind = kind;
    link->byte_offset = offset;
    link->byte_length = length;
    (void)memcpy(link->target, source + offset, length);
    link->target[length] = '\0';
    link->line = line;
    link->column = column;
    return UMI_STATUS_OK;
}

/* Provide the trim punctuation operation used by this module and its client applications. */
static void trim_punctuation(const char *text, size_t *length)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*length > 0U) {
        char value = text[*length - 1U];
        /* Apply this branch only when its contract condition is satisfied. */
        if (value != '.' && value != ',' && value != ';' && value != ':') break;
        *length -= 1U;
    }
}

/* Provide the parse location operation used by this module and its client applications. */
static int parse_location(const char *token,
                          size_t length,
                          size_t *path_length,
                          uint32_t *line,
                          uint32_t *column)
{
    size_t last = SIZE_MAX;
    size_t previous = SIZE_MAX;
    size_t index;
    char *end = NULL;
    unsigned long parsed_line;
    unsigned long parsed_column = 0UL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (token[index] == ':') {
            previous = last;
            last = index;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (last == SIZE_MAX || last + 1U >= length ||
        !isdigit((unsigned char)token[last + 1U])) return 0;
    parsed_line = strtoul(token + last + 1U, &end, 10);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (end != token + length) {
        return 0;
    }
    *path_length = last;
    *line = (uint32_t)parsed_line;
    *column = 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (previous != SIZE_MAX && previous > 1U &&
        isdigit((unsigned char)token[previous + 1U])) {
        parsed_line = strtoul(token + previous + 1U, &end, 10);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (end == token + last) {
            parsed_column = strtoul(token + last + 1U, &end, 10);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (end == token + length) {
                *path_length = previous;
                *line = (uint32_t)parsed_line;
                *column = (uint32_t)parsed_column;
            }
        }
    }
    return *path_length > 0U;
}

/*
 * Provide the terminal links detect operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_links_detect(const char *text,
                                    UmiTerminalLinkResult *out_result)
{
    size_t offset = 0U;
    size_t text_length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    text_length = strlen(text);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (offset < text_length) {
        size_t start;
        size_t length;
        size_t path_length;
        uint32_t line = 0U;
        uint32_t column = 0U;
        UmiTerminalLinkKind kind = UMI_TERMINAL_LINK_FILE;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (offset < text_length && terminal_delimiter(text[offset])) offset += 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (offset >= text_length) break;
        start = offset;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (offset < text_length && !terminal_delimiter(text[offset])) offset += 1U;
        length = offset - start;
        trim_punctuation(text + start, &length);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length == 0U) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if ((length >= 7U && strncmp(text + start, "http://", 7U) == 0) ||
            (length >= 8U && strncmp(text + start, "https://", 8U) == 0)) {
            kind = UMI_TERMINAL_LINK_WEB;
        } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (memchr(text + start, '@', length) != NULL &&
                   memchr(text + start, '.', length) != NULL) {
            kind = UMI_TERMINAL_LINK_EMAIL;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (parse_location(text + start, length, &path_length,
                                  &line, &column)) {
            kind = UMI_TERMINAL_LINK_COMPILER_LOCATION;
            length = path_length;
        } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (memchr(text + start, '/', length) == NULL &&
                   memchr(text + start, '\\', length) == NULL) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (add_link(out_result, kind, text, start, length, line, column) !=
            UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the terminal link kind text operation used by this module and its client
 * applications.
 */
const char *umi_terminal_link_kind_text(UmiTerminalLinkKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_TERMINAL_LINK_WEB: return "web";
        case UMI_TERMINAL_LINK_FILE: return "file";
        case UMI_TERMINAL_LINK_COMPILER_LOCATION: return "compiler-location";
        case UMI_TERMINAL_LINK_EMAIL: return "email";
        default: return "unknown";
    }
}
