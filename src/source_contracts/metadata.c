/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/metadata.c
 *
 * PURPOSE:
 *   Accept meaningful separate or combined attribution without rewriting source comments.
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

typedef enum ScField { FIELD_NONE, FIELD_FILE, FIELD_PURPOSE, FIELD_AUTHOR,
    FIELD_ORGANISATION, FIELD_COMBINED, FIELD_LICENCE } ScField;

static ScField fieldFor(const char *begin, size_t n)
{
    char key[96]; size_t used = 0U;
    for (size_t i = 0U; i < n; ++i) {
        unsigned char c = (unsigned char)begin[i];
        if (isspace(c) || c == '-' || c == '_') continue;
        if (used + 1U == sizeof(key)) return FIELD_NONE;
        key[used++] = (char)tolower(c);
    }
    key[used] = '\0';
    if (strcmp(key, "file") == 0) return FIELD_FILE;
    if (strcmp(key, "purpose") == 0) return FIELD_PURPOSE;
    if (strcmp(key, "author") == 0 || strcmp(key, "createdby") == 0) return FIELD_AUTHOR;
    if (strcmp(key, "organisation") == 0 || strcmp(key, "organization") == 0) return FIELD_ORGANISATION;
    if (strcmp(key, "authorandorganisation") == 0 || strcmp(key, "authorandorganization") == 0) return FIELD_COMBINED;
    if (strcmp(key, "licence") == 0 || strcmp(key, "license") == 0 || strcmp(key, "spdxlicenseidentifier") == 0) return FIELD_LICENCE;
    return FIELD_NONE;
}
static void recordValue(UmiSourceAttribution *out, ScField field,
    const char *begin, const char *end)
{
    bool nonempty = false;
    for (const char *p = begin; p < end; ++p)
        if (!isspace((unsigned char)*p) && *p != '*' && *p != '-' && *p != '/') { nonempty = true; break; }
    if (!nonempty) return;
    switch (field) {
    case FIELD_FILE: out->file = true; break;
    case FIELD_PURPOSE: out->purpose = true; break;
    case FIELD_AUTHOR: out->author = true; break;
    case FIELD_ORGANISATION: out->organisation = true; break;
    case FIELD_COMBINED: out->combined = true; break;
    case FIELD_LICENCE: out->licence = true; break;
    default: break;
    }
}

int UmiSourceContractReadAttribution(const char *source, size_t length,
    UmiSourceAttribution *out)
{
    if (source == NULL || out == NULL || length > SC_MAX_FILE_BYTES) return 2;
    memset(out, 0, sizeof(*out));
    if (memchr(source, '\0', length) != NULL) return 2;
    size_t begin = 0U;
    if (length >= 3U && memcmp(source, "\xEF\xBB\xBF", 3U) == 0) begin = 3U;
    while (begin < length && isspace((unsigned char)source[begin])) ++begin;
    if (begin + 1U >= length || source[begin] != '/' || source[begin + 1U] != '*') return 1;
    size_t end = begin + 2U;
    while (end + 1U < length && !(source[end] == '*' && source[end + 1U] == '/')) ++end;
    if (end + 1U >= length) return 2;
    out->leading_comment = true;
    ScField field = FIELD_NONE;
    size_t fieldIndent = 0U;
    for (size_t i = begin + 2U; i < end;) {
        size_t finish = i;
        while (finish < end && source[finish] != '\n' && source[finish] != '\r') ++finish;
        const char *a = source + i, *b = source + finish;
        size_t indent = 0U;
        while (a < b && (isspace((unsigned char)*a) || *a == '*')) {
            indent = *a == '*' ? 0U : indent + 1U;
            ++a;
        }
        while (b > a && isspace((unsigned char)b[-1])) --b;
        const char *colon = memchr(a, ':', (size_t)(b - a));
        bool label = colon != NULL;
        /* URLs and value punctuation do not become metadata headings. */
        if (colon != NULL) {
            for (const char *p = a; p < colon; ++p)
                if (!isalpha((unsigned char)*p) && !isspace((unsigned char)*p) && *p != '-' && *p != '_') label = false;
        }
        /* A more-indented continuation may contain prose such as
         * "Validate behaviour: acceptance pending". It is a value, not an
         * unknown heading. Recognised labels still start a new field; an
         * unknown heading at the current margin still ends attribution. */
        if (label && field != FIELD_NONE && indent > fieldIndent &&
            fieldFor(a, (size_t)(colon - a)) == FIELD_NONE) label = false;
        if (label) {
            field = fieldFor(a, (size_t)(colon - a));
            fieldIndent = indent;
            recordValue(out, field, colon + 1, b);
        } else recordValue(out, field, a, b);
        i = finish;
        while (i < end && (source[i] == '\n' || source[i] == '\r')) ++i;
    }
    return 0;
}
int ScMetadata(const char *source, size_t length, const char *path,
    UmiSourceContractReport *report)
{
    UmiSourceAttribution result;
    int parsed = UmiSourceContractReadAttribution(source, length, &result);
    if (parsed != 0) {
        ScFinding(report, path, 1U, "metadata.comment", parsed == 2 ? "Malformed leading comment or input" : "Missing leading file comment", parsed == 2);
        return parsed;
    }
    struct { bool present; const char *rule; const char *message; } checks[] = {
        {result.file, "metadata.file", "Missing nonempty File field"},
        {result.purpose, "metadata.purpose", "Missing nonempty Purpose field"},
        {result.author || result.organisation || result.combined, "metadata.attribution", "Provide a nonempty Author OR Organisation (separate or combined fields)"},
        {result.licence, "metadata.licence", "Missing nonempty Licence field; preserve the actual licence"}
    };
    for (size_t i = 0U; i < sizeof(checks) / sizeof(checks[0]); ++i) {
        ++report->checks;
        if (!checks[i].present) ScFinding(report, path, 1U, checks[i].rule, checks[i].message, false);
    }
    return 0;
}

int ScGuard(const char *source, const char *path, char **guard,
    UmiSourceContractReport *report)
{
    ScText text;
    *guard = NULL;
    if (ScLex(&text, source, false) != 0) {
        ScFinding(report, path, 1U, "header.parse", "Unable to tokenise header", true); return 2;
    }
    ++report->checks;
    size_t at = ScIndex(&text, "# ifndef", 0U);
    if (at == SIZE_MAX || !ScIdentifier(&text, at + 2U)) {
        ScFinding(report, path, 1U, "header.guard", "Missing include guard", false);
        ScTextFree(&text); return 1;
    }
    ScToken token = text.tokens[at + 2U];
    *guard = ScCopy(text.data + token.begin, token.end - token.begin);
    if (*guard == NULL) { ScTextFree(&text); return 2; }
    bool defined = false;
    for (size_t i = at + 3U; i + 2U < text.count; ++i)
        if (ScTokenEquals(&text, i, "#") && ScTokenEquals(&text, i + 1U, "define") && ScTokenEquals(&text, i + 2U, *guard)) { defined = true; break; }
    if (!defined) ScFinding(report, path, ScLine(&text, at), "header.guard", "#ifndef and #define do not match", false);
    ScTextFree(&text); return defined ? 0 : 1;
}
