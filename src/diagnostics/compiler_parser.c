/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/compiler_parser.c
 *
 * PURPOSE:
 *   Implement the compiler parser behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/compiler_parser.h"
#include "compiler_parser_internal.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Provide the compiler hash operation used by this module and its client applications. */
static uint64_t compiler_hash(const char *text)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    const unsigned char *cursor = (const unsigned char *)text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor != NULL && *cursor != 0U) {
        hash ^= (uint64_t)*cursor++;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}


/* Copy only complete fields. A cropped path can name a different source file. */
static int CopyField(char *out, size_t capacity, const char *first, const char *last)
{
    size_t length;
    while (first < last && (*first == ' ' || *first == '\t')) ++first;
    while (last > first && (last[-1] == ' ' || last[-1] == '\t' || last[-1] == '\r')) --last;
    length = (size_t)(last - first);
    if (length >= capacity) return 0;
    memcpy(out, first, length);
    out[length] = '\0';
    return 1;
}

/* Decimal accumulation avoids scanf/strtoul overflow and rejects signs. */
static int ParsePosition(const char *first, const char *last, size_t *out)
{
    size_t value = 0U;
    if (first == last) return 0;
    for (const char *p = first; p < last; ++p) {
        size_t digit;
        if (*p < '0' || *p > '9') return 0;
        digit = (size_t)(*p - '0');
        if (value > (SIZE_MAX - digit) / 10U) return 0;
        value = value * 10U + digit;
    }
    if (value == 0U) return 0;
    *out = value;
    return 1;
}

static const char *LastDelimiter(const char *first, const char *last, char delimiter)
{
    while (last > first) {
        --last;
        if (*last == delimiter) return last;
    }
    return NULL;
}

/* Recognise suffixes from the right, so drive letters and parentheses in
 * directory names remain part of the path. Failure leaves position unknown. */
static UmiStatus ParseLocation(const char *first, const char *last,
    UmiCompilerDiagnosticFields *fields)
{
    const char *pathEnd = NULL;
    size_t line = 0U, column = 0U;
    while (last > first && (last[-1] == ' ' || last[-1] == '\t')) --last;
    if (last > first && last[-1] == ':') --last;
    while (last > first && (last[-1] == ' ' || last[-1] == '\t')) --last;
    if (last == first) return UMI_STATUS_NOT_FOUND;
    if (last[-1] == ')') {
        const char *open = LastDelimiter(first, last - 1, '(');
        if (open != NULL) {
            const char *comma = memchr(open + 1, ',', (size_t)(last - 1 - (open + 1)));
            if (comma != NULL) {
                if (!ParsePosition(open + 1, comma, &line) ||
                    !ParsePosition(comma + 1, last - 1, &column)) return UMI_STATUS_NOT_FOUND;
            } else if (!ParsePosition(open + 1, last - 1, &line)) return UMI_STATUS_NOT_FOUND;
            pathEnd = open;
        }
    } else {
        const char *right = LastDelimiter(first, last, ':');
        if (right != NULL && ParsePosition(right + 1, last, &line)) {
            const char *left = LastDelimiter(first, right, ':');
            size_t candidate;
            pathEnd = right;
            if (left != NULL && ParsePosition(left + 1, right, &candidate)) {
                column = line;
                line = candidate;
                pathEnd = left;
            } else if (left != NULL && right > left + 1 &&
                (isdigit((unsigned char)left[1]) || left[1] == '-' || left[1] == '+')) {
                /* A malformed numeric line is not a filename suffix. */
                return UMI_STATUS_NOT_FOUND;
            }
        }
    }
    if (pathEnd == NULL || pathEnd == first) return UMI_STATUS_NOT_FOUND;
    if (!CopyField(fields->path, sizeof fields->path, first, pathEnd))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (fields->path[0] == '\0') return UMI_STATUS_NOT_FOUND;
    fields->line = line;
    fields->column = column;
    return UMI_STATUS_OK;
}

/* This is a colour stripper, not a terminal emulator. Unsupported or
 * unterminated escapes make the line unsuitable for source navigation. */
static UmiStatus CleanLine(const char *text, char *clean, size_t capacity)
{
    size_t used = 0U, read = 0U;
    while (text[read] != '\0' && text[read] != '\n') {
        unsigned char c = (unsigned char)text[read++];
        if (read > 65536U) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (c == 27U) {
            if (text[read] != '[') return UMI_STATUS_NOT_FOUND;
            ++read;
            while (text[read] != '\0' &&
                (isdigit((unsigned char)text[read]) || text[read] == ';' || text[read] == ':')) {
                if (++read > 65536U) return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            if (text[read] != 'm' && text[read] != 'K') return UMI_STATUS_NOT_FOUND;
            ++read;
            continue;
        }
        if (c < 32U && c != '\t' && c != '\r') return UMI_STATUS_NOT_FOUND;
        if (used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        clean[used++] = (char)c;
    }
    while (used != 0U && clean[used - 1U] == '\r') --used;
    clean[used] = '\0';
    return UMI_STATUS_OK;
}

static int LevelAt(const char *p, UmiDiagnosticSeverity *severity,
    const char **after)
{
    static const struct { const char *name; UmiDiagnosticSeverity value; } levels[] = {
        {"fatal error", UMI_DIAGNOSTIC_FATAL}, {"error", UMI_DIAGNOSTIC_ERROR},
        {"warning", UMI_DIAGNOSTIC_WARNING}, {"note", UMI_DIAGNOSTIC_INFO}
    };
    for (size_t n = 0U; n < sizeof levels / sizeof levels[0]; ++n) {
        size_t length = strlen(levels[n].name);
        if (strncmp(p, levels[n].name, length) == 0 &&
            (p[length] == ':' || p[length] == ' ' || p[length] == '\t')) {
            *severity = levels[n].value;
            *after = p + length;
            return 1;
        }
    }
    return 0;
}

static UmiStatus ParseCMake(const char *text, UmiCompilerDiagnosticFields *fields)
{
    const char *location, *command, *end;
    /* A table of complete prefixes extends the existing Error/Warning/dev
     * handling to author and deprecation diagnostics without suppressing them.
     * The old dispatch remains below for public review. Author warnings use
     * the same canonical location path, not a second application parser. */
#if 0
    if (strncmp(text, "CMake Error at ", 15U) == 0) {
        fields->severity = UMI_DIAGNOSTIC_ERROR; location = text + 15U;
    } else if (strncmp(text, "CMake Warning at ", 17U) == 0) {
        fields->severity = UMI_DIAGNOSTIC_WARNING; location = text + 17U;
    } else if (strncmp(text, "CMake Warning (dev) at ", 23U) == 0) {
        fields->severity = UMI_DIAGNOSTIC_WARNING; location = text + 23U;
    } else return UMI_STATUS_NOT_FOUND;
#endif
    static const struct {
        const char *prefix;
        UmiDiagnosticSeverity severity;
    } headers[] = {
        {"CMake Error at ", UMI_DIAGNOSTIC_ERROR},
        {"CMake Warning at ", UMI_DIAGNOSTIC_WARNING},
        {"CMake Warning (dev) at ", UMI_DIAGNOSTIC_WARNING},
        {"CMake Warning (author) at ", UMI_DIAGNOSTIC_WARNING},
        {"CMake Deprecation Warning at ", UMI_DIAGNOSTIC_WARNING},
        {"CMake Deprecation Error at ", UMI_DIAGNOSTIC_ERROR}
    };
    location = NULL;
    for (size_t i = 0U; i < sizeof headers / sizeof headers[0]; ++i) {
        size_t length = strlen(headers[i].prefix);
        if (strncmp(text, headers[i].prefix, length) == 0) {
            fields->severity = headers[i].severity;
            location = text + length;
            break;
        }
    }
    if (location == NULL) return UMI_STATUS_NOT_FOUND;
    end = text + strlen(text);
    command = LastDelimiter(location, end, '(');
    if (command != NULL && command > location && command[-1] == ' ') --command;
    else command = end;
    UmiStatus status = ParseLocation(location, command, fields);
    if (status != UMI_STATUS_OK) return status;
    /* CMake commonly puts its explanation on subsequent lines. Keep the
     * complete header visible and leave continuation text in the transcript. */
    return CopyField(fields->message, sizeof fields->message, text, end)
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

UmiStatus UmiCompilerDiagnosticParseText(const char *text,
    UmiCompilerDiagnosticFields *outFields)
{
    char clean[8192];
    UmiCompilerDiagnosticFields fields = {0};
    UmiStatus status;
    const char *first, *end;
    if (text == NULL || outFields == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = CleanLine(text, clean, sizeof clean);
    if (status != UMI_STATUS_OK) return status;
    first = clean;
    while (*first == ' ' || *first == '\t') ++first;
    end = first + strlen(first);
    status = ParseCMake(first, &fields);
    if (status == UMI_STATUS_OK) { *outFields = fields; return status; }
    if (status != UMI_STATUS_NOT_FOUND) return status;
    memset(&fields, 0, sizeof fields);
    for (const char *marker = first; marker < end; ++marker) {
        const char *after, *codeStart, *message;
        if (marker != first && marker[-1] != ':' && marker[-1] != ' ' && marker[-1] != '\t') continue;
        if (!LevelAt(marker, &fields.severity, &after)) continue;
        while (*after == ' ' || *after == '\t') ++after;
        codeStart = after;
        /* MSVC inserts its error identifier between severity and colon. */
        while (isalnum((unsigned char)*after) || *after == '_' || *after == '-') ++after;
        if (*after != ':') continue;
        if (after != codeStart && !CopyField(fields.code, sizeof fields.code, codeStart, after))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        message = after + 1;
        while (*message == ' ' || *message == '\t') ++message;
        if (message == end) return UMI_STATUS_NOT_FOUND;
        status = ParseLocation(first, marker, &fields);
        if (status == UMI_STATUS_CAPACITY_EXCEEDED) return status;
        if (status != UMI_STATUS_OK && marker != first) {
            const char *prefixEnd = marker;
            while (prefixEnd > first && (prefixEnd[-1] == ' ' || prefixEnd[-1] == '\t')) --prefixEnd;
            /* Tool errors such as "clang: error:" are useful without a file.
             * Source excerpts and success summaries are not diagnostics. */
            if (prefixEnd == first || prefixEnd[-1] != ':') continue;
            fields.path[0] = '\0'; fields.line = 0U; fields.column = 0U;
        }
        if (!CopyField(fields.message, sizeof fields.message, message, end))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        if (fields.code[0] == '\0' && end > message && end[-1] == ']') {
            const char *open = LastDelimiter(message, end, '[');
            if (open != NULL && end - open > 3 && open[1] == '-' && open[2] == 'W' &&
                !CopyField(fields.code, sizeof fields.code, open + 1, end - 1))
                return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        *outFields = fields;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}


/* A build transcript is a sequence of records. CMake's explanation belongs to
 * its preceding header, while its call stack remains separate transcript text.
 * Keeping this grouping beside the compiler grammar lets every build consumer
 * use the same source location and message instead of guessing in the UI. */
static size_t DiagnosticLineBytes(const char *text)
{
    const char *newline = strchr(text, '\n');
    return newline != NULL ? (size_t)(newline - text) + 1U : strlen(text);
}

UmiStatus UmiCompilerDiagnosticParseBlock(const char *text,
    UmiCompilerDiagnosticFields *outFields, size_t *outConsumed)
{
    if (text == NULL || outFields == NULL || outConsumed == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCompilerDiagnosticFields candidate;
    const size_t firstBytes = DiagnosticLineBytes(text);
    *outConsumed = firstBytes;
    UmiStatus status = UmiCompilerDiagnosticParseText(text, &candidate);
    if (status != UMI_STATUS_OK) return status;

    /* The single-line parser has already recognised this complete header.
     * Other compiler formats keep their established one-line behaviour. */
    char header[8192];
    UmiCompilerDiagnosticFields cmakeHeader = {0};
    status = CleanLine(text, header, sizeof header);
    if (status != UMI_STATUS_OK) return status;
    const char *headerStart = header;
    while (*headerStart == ' ' || *headerStart == '\t') ++headerStart;
    if (ParseCMake(headerStart, &cmakeHeader) != UMI_STATUS_OK) {
        *outFields = candidate;
        return UMI_STATUS_OK;
    }

    const char *cursor = text + firstBytes;
    size_t used = strlen(candidate.message);
    int paragraph = 0;
    UmiStatus recordStatus = UMI_STATUS_OK;
    while (*cursor != '\0') {
        char clean[8192];
        size_t bytes = DiagnosticLineBytes(cursor);
        status = CleanLine(cursor, clean, sizeof clean);
        if (status != UMI_STATUS_OK) {
            /* An oversized indented line still belongs to the record. Consume
             * it without publishing a shortened message or parsing its tail as
             * another error. Unsupported terminal controls remain raw output. */
            if (status != UMI_STATUS_CAPACITY_EXCEEDED ||
                (cursor[0] != ' ' && cursor[0] != '\t')) break;
            recordStatus = UMI_STATUS_CAPACITY_EXCEEDED;
            cursor += bytes;
            *outConsumed = (size_t)(cursor - text);
            continue;
        }

        const char *first = clean;
        while (*first == ' ' || *first == '\t') ++first;
        if (*first == '\0') {
            /* Blank separators join the record only when followed by another
             * indented paragraph. Trailing separators stay in the transcript. */
            paragraph = 1;
            cursor += bytes;
            continue;
        }
        if (first == clean) break;

        UmiCompilerDiagnosticFields nextHeader = {0};
        if (ParseCMake(first, &nextHeader) != UMI_STATUS_NOT_FOUND) break;
        const char *last = first + strlen(first);
        while (last > first && (last[-1] == ' ' || last[-1] == '\t')) --last;
        size_t length = (size_t)(last - first);
        size_t separator = paragraph ? 2U : 1U;
        if (recordStatus == UMI_STATUS_OK) {
            /* Reserve the terminator before subtracting: an exact fit is valid,
             * but no partial UTF-8 text or ellipsis is substituted on overflow. */
            size_t remaining = sizeof candidate.message - 1U - used;
            if (separator > remaining || length > remaining - separator) {
                recordStatus = UMI_STATUS_CAPACITY_EXCEEDED;
            } else {
                candidate.message[used++] = '\n';
                if (paragraph) candidate.message[used++] = '\n';
                memcpy(candidate.message + used, first, length);
                used += length;
                candidate.message[used] = '\0';
            }
        }
        paragraph = 0;
        cursor += bytes;
        *outConsumed = (size_t)(cursor - text);
    }
    if (recordStatus == UMI_STATUS_OK) *outFields = candidate;
    return recordStatus;
}

/* Keep the original registry entry point. Only the field grammar is shared. */
UmiStatus umi_compiler_diagnostic_parse(const UmiOutputRecord *output,
    UmiDiagnosticSnapshot *out_diagnostic, int *out_matched, void *user_data)
{
    UmiCompilerDiagnosticFields fields;
    UmiDiagnosticSnapshot candidate;
    char identifier[UMI_DIAGNOSTIC_ID_CAPACITY];
    UmiStatus status;
    (void)user_data;
    if (output == NULL || out_diagnostic == NULL || out_matched == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_matched = 0;
    if (memchr(output->text, '\0', sizeof output->text) == NULL ||
        memchr(output->source, '\0', sizeof output->source) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiCompilerDiagnosticParseText(output->text, &fields);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    snprintf(identifier, sizeof identifier, "compiler-%016" PRIx64,
        compiler_hash(output->text) ^ output->correlation_id);
    status = umi_diagnostic_snapshot_init(&candidate, identifier, fields.severity,
        UMI_DIAGNOSTIC_KIND_COMPILER, output->source[0] != '\0' ? output->source : "compiler",
        fields.message);
    if (status != UMI_STATUS_OK) return status;
    /* Do not turn a longer valid path into a shorter, wrong file location. */
    if (strlen(fields.path) >= sizeof candidate.uri || fields.line > UINT32_MAX || fields.column > UINT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(candidate.uri, fields.path);
    strcpy(candidate.code, fields.code[0] != '\0' ? fields.code :
        fields.severity == UMI_DIAGNOSTIC_FATAL ? "fatal-error" :
        fields.severity == UMI_DIAGNOSTIC_ERROR ? "error" :
        fields.severity == UMI_DIAGNOSTIC_WARNING ? "warning" : "note");
    candidate.line = (uint32_t)fields.line; candidate.column = (uint32_t)fields.column;
    candidate.correlation_id = output->correlation_id; candidate.timestamp_ns = output->timestamp_ns;
    if (fields.path[0] == '\0' && strstr(output->text, "link") != NULL)
        candidate.kind = UMI_DIAGNOSTIC_KIND_LINKER;
    *out_diagnostic = candidate; *out_matched = 1;
    return UMI_STATUS_OK;
}

/* Provide the compiler diagnostic parser operation used by this module and its client
 * applications. */
UmiDiagnosticParser umi_compiler_diagnostic_parser(void)
{
    UmiDiagnosticParser parser = { "umicom.compiler", 100, umi_compiler_diagnostic_parse, NULL };
    return parser;
}

/* Test locations share the compiler's colour handling and numeric grammar.
 * Keeping the extension beside those helpers prevents IDE-specific parsing
 * from drifting across build diagnostics and selected-test evidence. */
#include "umicom/diagnostics/failure_parser.h"
static int FailureLocalPath(const char *path)
{
    int shaped = 0;
    for (size_t i = 0U; path[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)path[i];
        if (c < 32U || c == 127U || c == '"' || c == '<' || c == '>') return 0;
        if (c == ':' && !(i == 1U && isalpha((unsigned char)path[0]) &&
            (path[2] == '/' || path[2] == '\\'))) return 0;
        if (c == '/' || c == '\\' || c == '.') shaped = 1;
    }
    return shaped;
}
UmiStatus UmiTestFailureParseText(const char *text, UmiCompilerDiagnosticFields *outFields)
{
    if (text == NULL || outFields == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char clean[8192];
    UmiStatus status = CleanLine(text, clean, sizeof(clean));
    if (status != UMI_STATUS_OK) return status;
    const char *first = clean;
    while (*first == ' ' || *first == '\t') ++first;
    const char *number = first;
    while (*number >= '0' && *number <= '9') ++number;
    if (number != first && number[0] == ':' && (number[1] == ' ' || number[1] == '\t')) {
        first = number + 1;
        while (*first == ' ' || *first == '\t') ++first;
    }
    UmiCompilerDiagnosticFields candidate = {0};
    status = UmiCompilerDiagnosticParseText(first, &candidate);
    if (status == UMI_STATUS_OK) {
        if (!FailureLocalPath(candidate.path)) return UMI_STATUS_NOT_FOUND;
        *outFields = candidate; return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_NOT_FOUND) return status;
    const char *end = first + strlen(first);
    for (const char *separator = first; separator < end; ++separator) {
        if (*separator != ':' || (separator[1] != ' ' && separator[1] != '\t' && separator[1] != '\0')) continue;
        memset(&candidate, 0, sizeof(candidate));
        status = ParseLocation(first, separator, &candidate);
        if (status == UMI_STATUS_CAPACITY_EXCEEDED) return status;
        if (status != UMI_STATUS_OK || !FailureLocalPath(candidate.path)) continue;
        if (!CopyField(candidate.message, sizeof(candidate.message), separator + 1, end)) return UMI_STATUS_CAPACITY_EXCEEDED;
        candidate.severity = UMI_DIAGNOSTIC_ERROR;
        *outFields = candidate; return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Stream consumers need the same cleaned indentation and CMake header grammar
 * as ParseBlock. Keeping this small boundary query beside that grammar prevents
 * byte-chunk boundaries from changing the meaning of a compiler transcript. */
UmiCompilerLineKind UmiCompilerClassifyLine(const char *line)
{
    char clean[8192];
    UmiCompilerDiagnosticFields header = {0};
    if (line == NULL || CleanLine(line, clean, sizeof clean) != UMI_STATUS_OK)
        return UMI_COMPILER_LINE_OTHER;
    const char *first = clean;
    while (*first == ' ' || *first == '\t') ++first;
    if (*first == '\0') return UMI_COMPILER_LINE_BLANK;
    if (ParseCMake(first, &header) != UMI_STATUS_NOT_FOUND)
        return UMI_COMPILER_LINE_CMAKE;
    return first != clean ? UMI_COMPILER_LINE_INDENTED : UMI_COMPILER_LINE_OTHER;
}
