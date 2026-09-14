/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/text.c
 *
 * PURPOSE:
 *   Tokenise source without confusing comments, strings, nested calls or casts.
 *
 * Author: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

char *ScCopy(const char *text, size_t length)
{
    if (text == NULL || length == SIZE_MAX) return NULL;
    char *copy = malloc(length + 1U);
    if (copy != NULL) { memcpy(copy, text, length); copy[length] = '\0'; }
    return copy;
}

void ScFinding(UmiSourceContractReport *report, const char *path, size_t line,
    const char *rule, const char *message, bool input_error)
{
    if (report == NULL) return;
    ++report->findings;
    if (input_error) ++report->input_errors;
    if (report->diagnostic != NULL)
        report->diagnostic(report->context, path, line, rule, message);
}

/* Token spans retain original line locations. Neither comments nor quoted
 * delimiters participate in balancing. Spliced physical lines are accepted. */
int ScLex(ScText *text, const char *data, bool cmake)
{
    if (text == NULL || data == NULL) return 2;
    memset(text, 0, sizeof(*text));
    text->length = strlen(data);
    if (text->length > SC_MAX_FILE_BYTES) return 2;
    text->data = ScCopy(data, text->length);
    size_t capacity = 256U;
    text->tokens = malloc(capacity * sizeof(*text->tokens));
    if (text->data == NULL || text->tokens == NULL) goto invalid;
    size_t i = 0U;
    if (text->length >= 3U && memcmp(data, "\xEF\xBB\xBF", 3U) == 0) i = 3U;
    while (i < text->length) {
        unsigned char c = (unsigned char)data[i];
        if (isspace(c)) { ++i; continue; }
        if (c == '\\' && i + 1U < text->length && data[i + 1U] == '\n') {
            i += 2U; continue;
        }
        if (c == '\\' && i + 2U < text->length && data[i + 1U] == '\r' && data[i + 2U] == '\n') {
            i += 3U; continue;
        }
        if (cmake && c == '#') {
            /* CMake bracket comments may include several lines. */
            size_t p = i + 1U;
            if (data[p] == '[') {
                size_t equals = 0U; ++p;
                while (data[p] == '=') { ++equals; ++p; }
                if (data[p] == '[') {
                    ++p; bool closed = false;
                    while (p < text->length) {
                        if (data[p] == ']') {
                            size_t q = p + 1U, n = 0U;
                            while (q < text->length && data[q] == '=') { ++q; ++n; }
                            if (n == equals && q < text->length && data[q] == ']') {
                                i = q + 1U; closed = true; break;
                            }
                        }
                        ++p;
                    }
                    if (!closed) goto invalid;
                    continue;
                }
            }
            while (i < text->length && data[i] != '\n') ++i;
            continue;
        }
        if (!cmake && c == '/' && i + 1U < text->length) {
            if (data[i + 1U] == '/') {
                i += 2U;
                while (i < text->length && data[i] != '\n') {
                    if (data[i] == '\\' && i + 1U < text->length && data[i + 1U] == '\n') i += 2U;
                    else if (data[i] == '\\' && i + 2U < text->length && data[i + 1U] == '\r' && data[i + 2U] == '\n') i += 3U;
                    else ++i;
                }
                continue;
            }
            if (data[i + 1U] == '*') {
                i += 2U;
                while (i + 1U < text->length && !(data[i] == '*' && data[i + 1U] == '/')) ++i;
                if (i + 1U >= text->length) goto invalid;
                i += 2U; continue;
            }
        }
        size_t begin = i;
        unsigned char kind = 0U;
        if (c == '"' || (!cmake && c == '\'')) {
            kind = c; ++i; bool closed = false;
            while (i < text->length) {
                if (data[i] == '\\') {
                    if (i + 1U >= text->length) goto invalid;
                    i += 2U;
                } else if ((unsigned char)data[i] == c) { ++i; closed = true; break; }
                else ++i;
            }
            if (!closed) goto invalid;
        } else if (isalnum(c) || c == '_' || c >= 0x80U) {
            kind = 1U; ++i;
            while (i < text->length && (isalnum((unsigned char)data[i]) || data[i] == '_' || (unsigned char)data[i] >= 0x80U)) ++i;
        } else ++i;
        if (text->count == capacity) {
            if (capacity > SC_MAX_FILE_BYTES / 2U) goto invalid;
            capacity *= 2U;
            ScToken *next = realloc(text->tokens, capacity * sizeof(*next));
            if (next == NULL) goto invalid;
            text->tokens = next;
        }
        text->tokens[text->count++] = (ScToken){begin, i, kind};
    }
    return 0;
invalid:
    ScTextFree(text);
    return 2;
}

void ScTextFree(ScText *text)
{
    if (text == NULL) return;
    free(text->tokens); free(text->data); memset(text, 0, sizeof(*text));
}

bool ScTokenEquals(const ScText *text, size_t index, const char *value)
{
    if (text == NULL || index >= text->count || value == NULL) return false;
    ScToken t = text->tokens[index];
    size_t n = strlen(value);
    return t.end - t.begin == n && memcmp(text->data + t.begin, value, n) == 0;
}

bool ScIdentifier(const ScText *text, size_t index)
{
    if (text == NULL || index >= text->count || text->tokens[index].kind != 1U) return false;
    unsigned char c = (unsigned char)text->data[text->tokens[index].begin];
    return isalpha(c) || c == '_' || c >= 0x80U;
}

size_t ScMatching(const ScText *text, size_t opening)
{
    if (text == NULL || opening >= text->count) return SIZE_MAX;
    char expected[SC_MAX_DEPTH]; size_t depth = 0U;
    for (size_t i = opening; i < text->count; ++i) {
        ScToken t = text->tokens[i];
        if (t.kind != 0U || t.end != t.begin + 1U) continue;
        char c = text->data[t.begin], close = '\0';
        if (c == '(') close = ')'; else if (c == '[') close = ']'; else if (c == '{') close = '}';
        if (close != '\0') {
            if (depth == SC_MAX_DEPTH) return SIZE_MAX;
            expected[depth++] = close;
        } else if (c == ')' || c == ']' || c == '}') {
            if (depth == 0U || expected[depth - 1U] != c) return SIZE_MAX;
            if (--depth == 0U) return i;
        } else if (i == opening) return SIZE_MAX;
    }
    return SIZE_MAX;
}

int ScBody(const ScText *source, const char *name, ScText *body)
{
    memset(body, 0, sizeof(*body));
    for (size_t i = 0U; i + 1U < source->count; ++i) {
        if (!ScIdentifier(source, i) || !ScTokenEquals(source, i, name) || !ScTokenEquals(source, i + 1U, "(")) continue;
        size_t end = ScMatching(source, i + 1U);
        if (end == SIZE_MAX || !ScTokenEquals(source, end + 1U, "{")) continue;
        size_t last = ScMatching(source, end + 1U);
        if (last == SIZE_MAX) return 2;
        size_t begin = source->tokens[end + 1U].end;
        char *content = ScCopy(source->data + begin, source->tokens[last].begin - begin);
        if (content == NULL) return 2;
        int result = ScLex(body, content, false);
        if (result == 0) body->line_offset = ScLine(source, end + 1U) - 1U;
        free(content); return result;
    }
    return 1;
}

/* Token sequences distinguish executable syntax from a string that happens to
 * contain the same spelling. No regular expression is used for nested calls. */
size_t ScIndex(const ScText *source, const char *pattern, size_t from)
{
    ScText needle;
    if (ScLex(&needle, pattern, false) != 0) return SIZE_MAX;
    size_t found = SIZE_MAX;
    if (needle.count == 0U) { ScTextFree(&needle); return SIZE_MAX; }
    for (size_t i = from; i < source->count && needle.count <= source->count - i; ++i) {
        bool same = true;
        for (size_t j = 0U; j < needle.count; ++j) {
            ScToken a = source->tokens[i + j], b = needle.tokens[j];
            if (a.kind != b.kind || a.end - a.begin != b.end - b.begin ||
                memcmp(source->data + a.begin, needle.data + b.begin, a.end - a.begin) != 0) { same = false; break; }
        }
        if (same) { found = i; break; }
    }
    ScTextFree(&needle); return found;
}
bool ScContains(const ScText *source, const char *pattern) { return ScIndex(source, pattern, 0U) != SIZE_MAX; }
size_t ScCount(const ScText *source, const char *pattern)
{
    size_t count = 0U, from = 0U, at;
    while ((at = ScIndex(source, pattern, from)) != SIZE_MAX) { ++count; from = at + 1U; }
    return count;
}
char *ScCompactRange(const ScText *source, size_t first, size_t last)
{
    if (first > last || last > source->count) return NULL;
    size_t n = 0U;
    for (size_t i = first; i < last; ++i) n += source->tokens[i].end - source->tokens[i].begin;
    char *out = malloc(n + 1U); if (out == NULL) return NULL;
    n = 0U;
    for (size_t i = first; i < last; ++i) {
        ScToken t = source->tokens[i]; size_t length = t.end - t.begin;
        memcpy(out + n, source->data + t.begin, length); n += length;
    }
    out[n] = '\0'; return out;
}
size_t ScLine(const ScText *source, size_t token)
{
    size_t line = source->line_offset + 1U, end = token < source->count ? source->tokens[token].begin : 0U;
    for (size_t i = 0U; i < end; ++i) if (source->data[i] == '\n') ++line;
    return line;
}
