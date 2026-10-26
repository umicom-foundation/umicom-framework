/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/literals.c
 *
 * PURPOSE:
 *   Find broken C literals and comments before metadata audits accept source text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Translation-phase line splicing precedes comment/literal recognition. Keep
 * a compact temporary copy so slash-star and slash-slash tokens split across
 * physical lines are handled consistently. Source remains caller-owned. */
static size_t ScSpliceBytes(const char *source, size_t length, size_t at) {
    if (source[at] != '\\' || at + 1U >= length) return 0U;
    if (source[at + 1U] == '\n') return 2U;
    return at + 2U < length && source[at + 1U] == '\r' && source[at + 2U] == '\n' ? 3U : 0U;
}
static size_t ScPhysicalLine(const char *source, size_t length, size_t logicalOffset) {
    size_t logical = 0U, line = 1U;
    for (size_t i = 0U; i < length;) {
        size_t splice = ScSpliceBytes(source, length, i);
        if (splice != 0U) { ++line; i += splice; continue; }
        if (logical == logicalOffset) return line;
        if (source[i] == '\n' || (source[i] == '\r' && (i + 1U == length || source[i + 1U] != '\n')))
            ++line;
        ++logical; ++i;
    }
    return line;
}
static int ScLiteralFinding(const char *source, size_t length, size_t at,
    const char *path, UmiSourceContractReport *report, const char *message) {
    ScFinding(report, path, ScPhysicalLine(source, length, at),
        "source.lexical", message, false);
    return 1;
}

/* This is a narrow lexical guard, not a C compiler or preprocessor. It does not
 * validate types, escape meanings, expressions, macro expansion or conditional
 * reachability. Even disabled C source must contain well-formed lexical tokens.
 * Numeric separators belong to C23 preprocessing numbers, not character literals. */
int ScLiterals(const char *source, const char *path, UmiSourceContractReport *report) {
    if (source == NULL || path == NULL || report == NULL) return 2;
    const size_t length = strlen(source);
    if (length > SC_MAX_FILE_BYTES) return 2;
    char *text = malloc(length + 1U);
    if (text == NULL) return 2;
    size_t used = 0U;
    for (size_t i = 0U; i < length;) {
        size_t splice = ScSpliceBytes(source, length, i);
        if (splice != 0U) i += splice;
        else text[used++] = source[i++];
    }
    text[used] = '\0';
    int result = 0;
    for (size_t i = 0U; i < used && result == 0;) {
        unsigned char c = (unsigned char)text[i];
        if (c == '/' && i + 1U < used && text[i + 1U] == '/') {
            i += 2U;
            while (i < used && text[i] != '\n' && text[i] != '\r') ++i;
        } else if (c == '/' && i + 1U < used && text[i + 1U] == '*') {
            size_t begin = i; i += 2U;
            while (i + 1U < used && !(text[i] == '*' && text[i + 1U] == '/')) ++i;
            if (i + 1U == used || i == used)
                result = ScLiteralFinding(source, length, begin, path, report, "Unterminated C block comment");
            else i += 2U;
        } else if (c == '"' || c == '\'') {
            size_t begin = i++;
            bool closed = false;
            while (i < used) {
                if (text[i] == '\n' || text[i] == '\r') {
                    result = ScLiteralFinding(source, length, i, path, report,
                        "Physical line break inside C literal; use an escaped newline or adjacent literals");
                    break;
                }
                if ((unsigned char)text[i] == c) { ++i; closed = true; break; }
                if (text[i] == '\\') {
                    if (i + 1U == used) break;
                    if (text[i + 1U] == '\n' || text[i + 1U] == '\r') {
                        result = ScLiteralFinding(source, length, i + 1U, path, report,
                            "Physical line break inside C literal");
                        break;
                    }
                    i += 2U;
                } else ++i;
            }
            if (result == 0 && !closed)
                result = ScLiteralFinding(source, length, begin, path, report, "Unterminated C literal");
        } else if (isdigit(c) || (c == '.' && i + 1U < used && isdigit((unsigned char)text[i + 1U]))) {
            ++i;
            while (i < used) {
                unsigned char next = (unsigned char)text[i];
                if (isalnum(next) || next == '_' || next == '.' || next >= 0x80U) ++i;
                else if (next == '\'' && i + 1U < used &&
                    (isalnum((unsigned char)text[i + 1U]) || text[i + 1U] == '_')) i += 2U;
                else if ((next == '+' || next == '-') && i > 0U &&
                    (text[i - 1U] == 'e' || text[i - 1U] == 'E' || text[i - 1U] == 'p' || text[i - 1U] == 'P')) ++i;
                else break;
            }
        } else if (isalpha(c) || c == '_' || c >= 0x80U) {
            ++i;
            while (i < used && (isalnum((unsigned char)text[i]) || text[i] == '_' ||
                                 (unsigned char)text[i] >= 0x80U)) ++i;
        } else ++i;
    }
    free(text);
    return result;
}
