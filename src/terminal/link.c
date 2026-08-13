/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/link.c
 *
 * PURPOSE:
 *   Detect common actionable links in terminal output with conservative,
 *   platform-neutral parsing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/link.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int terminal_delimiter(char value)
{
    return value == '\0' || isspace((unsigned char)value) || value == '"' ||
           value == '\'' || value == '<' || value == '>' || value == ')' ||
           value == ']' || value == '}';
}

static UmiStatus add_link(UmiTerminalLinkResult *result,
                          UmiTerminalLinkKind kind,
                          const char *source,
                          size_t offset,
                          size_t length,
                          uint32_t line,
                          uint32_t column)
{
    UmiTerminalLink *link;
    if (result->count >= UMI_TERMINAL_LINK_MAX) {
        result->truncated = 1;
        return UMI_STATUS_OK;
    }
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

static void trim_punctuation(const char *text, size_t *length)
{
    while (*length > 0U) {
        char value = text[*length - 1U];
        if (value != '.' && value != ',' && value != ';' && value != ':') break;
        *length -= 1U;
    }
}

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
    for (index = 0U; index < length; ++index) {
        if (token[index] == ':') {
            previous = last;
            last = index;
        }
    }
    if (last == SIZE_MAX || last + 1U >= length ||
        !isdigit((unsigned char)token[last + 1U])) return 0;
    parsed_line = strtoul(token + last + 1U, &end, 10);
    if (end != token + length) {
        return 0;
    }
    *path_length = last;
    *line = (uint32_t)parsed_line;
    *column = 0U;
    if (previous != SIZE_MAX && previous > 1U &&
        isdigit((unsigned char)token[previous + 1U])) {
        parsed_line = strtoul(token + previous + 1U, &end, 10);
        if (end == token + last) {
            parsed_column = strtoul(token + last + 1U, &end, 10);
            if (end == token + length) {
                *path_length = previous;
                *line = (uint32_t)parsed_line;
                *column = (uint32_t)parsed_column;
            }
        }
    }
    return *path_length > 0U;
}

UmiStatus umi_terminal_links_detect(const char *text,
                                    UmiTerminalLinkResult *out_result)
{
    size_t offset = 0U;
    size_t text_length;
    if (text == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    text_length = strlen(text);
    while (offset < text_length) {
        size_t start;
        size_t length;
        size_t path_length;
        uint32_t line = 0U;
        uint32_t column = 0U;
        UmiTerminalLinkKind kind = UMI_TERMINAL_LINK_FILE;
        while (offset < text_length && terminal_delimiter(text[offset])) offset += 1U;
        if (offset >= text_length) break;
        start = offset;
        while (offset < text_length && !terminal_delimiter(text[offset])) offset += 1U;
        length = offset - start;
        trim_punctuation(text + start, &length);
        if (length == 0U) continue;
        if ((length >= 7U && strncmp(text + start, "http://", 7U) == 0) ||
            (length >= 8U && strncmp(text + start, "https://", 8U) == 0)) {
            kind = UMI_TERMINAL_LINK_WEB;
        } else if (memchr(text + start, '@', length) != NULL &&
                   memchr(text + start, '.', length) != NULL) {
            kind = UMI_TERMINAL_LINK_EMAIL;
        } else if (parse_location(text + start, length, &path_length,
                                  &line, &column)) {
            kind = UMI_TERMINAL_LINK_COMPILER_LOCATION;
            length = path_length;
        } else if (memchr(text + start, '/', length) == NULL &&
                   memchr(text + start, '\\', length) == NULL) {
            continue;
        }
        if (add_link(out_result, kind, text, start, length, line, column) !=
            UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

const char *umi_terminal_link_kind_text(UmiTerminalLinkKind kind)
{
    switch (kind) {
        case UMI_TERMINAL_LINK_WEB: return "web";
        case UMI_TERMINAL_LINK_FILE: return "file";
        case UMI_TERMINAL_LINK_COMPILER_LOCATION: return "compiler-location";
        case UMI_TERMINAL_LINK_EMAIL: return "email";
        default: return "unknown";
    }
}
