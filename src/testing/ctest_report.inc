/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_report.inc
 *
 * PURPOSE:
 *   Parse bounded CTest JUnit records and escape exact literal test selections.
 *   Keep skipped, disabled and missing tests distinct from successful execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Private implementation fragment included once by ctest_adapter.c. Keeping the
 * parser beside its source owner avoids a second library or source-list entry.
 * This parser intentionally accepts CTest's small JUnit vocabulary only. */

typedef struct UmiCtestXmlCursor {
    const char *bytes;
    size_t length;
    size_t position;
} UmiCtestXmlCursor;

typedef struct UmiCtestXmlTag {
    char name[64];
    char keys[24][64];
    char values[24][1024];
    size_t count;
    int closing;
    int empty;
} UmiCtestXmlTag;

static int CtestXmlSpace(unsigned char value)
{
    return value == ' ' || value == '\t' || value == '\r' || value == '\n';
}

/* Validate one Unicode scalar rather than accepting overlong UTF-8 or surrogate
 * encodings. XML 1.0 excludes most control characters, even numeric entities. */
static int CtestXmlScalar(uint32_t value)
{
    return value == 9U || value == 10U || value == 13U ||
        (value >= 32U && value <= 0xD7FFU) ||
        (value >= 0xE000U && value <= 0xFFFDU) ||
        (value >= 0x10000U && value <= 0x10FFFFU);
}

static int CtestUtf8Read(const char *text, size_t length, size_t *used,
    uint32_t *value)
{
    if (length == 0U) return 0;
    unsigned char first = (unsigned char)text[0];
    size_t count;
    uint32_t result;
    uint32_t minimum;
    if (first < 0x80U) { count = 1U; result = first; minimum = 0U; }
    else if (first >= 0xC2U && first <= 0xDFU) {
        count = 2U; result = first & 0x1FU; minimum = 0x80U;
    } else if (first >= 0xE0U && first <= 0xEFU) {
        count = 3U; result = first & 0x0FU; minimum = 0x800U;
    } else if (first >= 0xF0U && first <= 0xF4U) {
        count = 4U; result = first & 7U; minimum = 0x10000U;
    } else return 0;
    if (count > length) return 0;
    for (size_t index = 1U; index < count; ++index) {
        unsigned char next = (unsigned char)text[index];
        if ((next & 0xC0U) != 0x80U) return 0;
        result = (result << 6U) | (uint32_t)(next & 0x3FU);
    }
    if (result < minimum || !CtestXmlScalar(result)) return 0;
    *used = count; *value = result;
    return 1;
}

static UmiStatus CtestNameLength(const char *name, size_t *outLength)
{
    if (name == NULL || name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_TEST_NAME_CAPACITY && name[length] != '\0') ++length;
    if (length == UMI_TEST_NAME_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < length;) {
        size_t used; uint32_t value;
        if (!CtestUtf8Read(name + index, length - index, &used, &value) ||
            value < 32U || value == 127U) return UMI_STATUS_INVALID_ARGUMENT;
        index += used;
    }
    *outLength = length;
    return UMI_STATUS_OK;
}

UmiStatus UmiCtestEscapeName(const char *testName, char *outPattern,
    size_t capacity)
{
    char pattern[UMI_CTEST_EXACT_PATTERN_CAPACITY];
    size_t length;
    UmiStatus status = CtestNameLength(testName, &length);
    if (status != UMI_STATUS_OK) return status;
    if (outPattern == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    size_t used = 0U;
    pattern[used++] = '^';
    for (size_t index = 0U; index < length; ++index) {
        if (strchr(".[](){}*+?^$|\\", testName[index]) != NULL)
            pattern[used++] = '\\';
        pattern[used++] = testName[index];
    }
    pattern[used++] = '$'; pattern[used] = '\0';
    if (used >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outPattern, pattern, used + 1U);
    return UMI_STATUS_OK;
}

static int CtestXmlStarts(const UmiCtestXmlCursor *cursor, const char *text)
{
    size_t length = strlen(text);
    return length <= cursor->length - cursor->position &&
        memcmp(cursor->bytes + cursor->position, text, length) == 0;
}

static void CtestXmlSkipSpace(UmiCtestXmlCursor *cursor)
{
    while (cursor->position < cursor->length &&
        CtestXmlSpace((unsigned char)cursor->bytes[cursor->position]))
        ++cursor->position;
}

static int CtestXmlNameChar(unsigned char value, int first)
{
    return (value >= 'A' && value <= 'Z') ||
        (value >= 'a' && value <= 'z') || value == '_' ||
        (!first && ((value >= '0' && value <= '9') ||
                    value == '-' || value == '.'));
}

static UmiStatus CtestXmlName(UmiCtestXmlCursor *cursor, char *name,
    size_t capacity)
{
    size_t used = 0U;
    while (cursor->position < cursor->length &&
        CtestXmlNameChar((unsigned char)cursor->bytes[cursor->position], used == 0U)) {
        if (used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        name[used++] = cursor->bytes[cursor->position++];
    }
    if (used == 0U) return UMI_STATUS_PARSE_ERROR;
    name[used] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus CtestXmlEntity(UmiCtestXmlCursor *cursor, uint32_t *value)
{
    static const struct { const char *name; uint32_t value; } entities[] = {
        {"amp;", '&'}, {"lt;", '<'}, {"gt;", '>'},
        {"quot;", '"'}, {"apos;", '\''}
    };
    if (!CtestXmlStarts(cursor, "&")) return UMI_STATUS_PARSE_ERROR;
    ++cursor->position;
    for (size_t index = 0U; index < sizeof entities / sizeof entities[0]; ++index) {
        if (CtestXmlStarts(cursor, entities[index].name)) {
            cursor->position += strlen(entities[index].name);
            *value = entities[index].value;
            return UMI_STATUS_OK;
        }
    }
    if (!CtestXmlStarts(cursor, "#")) return UMI_STATUS_PARSE_ERROR;
    ++cursor->position;
    uint32_t base = 10U, result = 0U;
    if (CtestXmlStarts(cursor, "x")) { base = 16U; ++cursor->position; }
    size_t digits = 0U;
    while (cursor->position < cursor->length &&
           cursor->bytes[cursor->position] != ';') {
        unsigned char character = (unsigned char)cursor->bytes[cursor->position++];
        uint32_t digit;
        if (character >= '0' && character <= '9') digit = character - '0';
        else if (base == 16U && character >= 'a' && character <= 'f')
            digit = (uint32_t)(character - 'a') + 10U;
        else if (base == 16U && character >= 'A' && character <= 'F')
            digit = (uint32_t)(character - 'A') + 10U;
        else return UMI_STATUS_PARSE_ERROR;
        if (digit >= base || result > (0x10FFFFU - digit) / base)
            return UMI_STATUS_PARSE_ERROR;
        result = result * base + digit; ++digits;
    }
    if (digits == 0U || !CtestXmlStarts(cursor, ";") || !CtestXmlScalar(result))
        return UMI_STATUS_PARSE_ERROR;
    ++cursor->position; *value = result;
    return UMI_STATUS_OK;
}

static UmiStatus CtestXmlAppendScalar(char *out, size_t capacity,
    size_t *used, uint32_t value)
{
    unsigned char bytes[4]; size_t count;
    if (value < 0x80U) { bytes[0] = (unsigned char)value; count = 1U; }
    else if (value < 0x800U) {
        bytes[0] = (unsigned char)(0xC0U | (value >> 6U));
        bytes[1] = (unsigned char)(0x80U | (value & 0x3FU)); count = 2U;
    } else if (value < 0x10000U) {
        bytes[0] = (unsigned char)(0xE0U | (value >> 12U));
        bytes[1] = (unsigned char)(0x80U | ((value >> 6U) & 0x3FU));
        bytes[2] = (unsigned char)(0x80U | (value & 0x3FU)); count = 3U;
    } else {
        bytes[0] = (unsigned char)(0xF0U | (value >> 18U));
        bytes[1] = (unsigned char)(0x80U | ((value >> 12U) & 0x3FU));
        bytes[2] = (unsigned char)(0x80U | ((value >> 6U) & 0x3FU));
        bytes[3] = (unsigned char)(0x80U | (value & 0x3FU)); count = 4U;
    }
    if (*used >= capacity || count >= capacity - *used)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out + *used, bytes, count); *used += count;
    return UMI_STATUS_OK;
}

static UmiStatus CtestXmlValue(UmiCtestXmlCursor *cursor, char *value,
    size_t capacity)
{
    if (cursor->position >= cursor->length) return UMI_STATUS_PARSE_ERROR;
    char quote = cursor->bytes[cursor->position++];
    if (quote != '\'' && quote != '"') return UMI_STATUS_PARSE_ERROR;
    size_t used = 0U;
    while (cursor->position < cursor->length &&
           cursor->bytes[cursor->position] != quote) {
        uint32_t scalar; size_t consumed;
        UmiStatus status;
        if (CtestXmlStarts(cursor, "&")) status = CtestXmlEntity(cursor, &scalar);
        else {
            if (cursor->bytes[cursor->position] == '<' ||
                !CtestUtf8Read(cursor->bytes + cursor->position,
                    cursor->length - cursor->position, &consumed, &scalar))
                return UMI_STATUS_PARSE_ERROR;
            cursor->position += consumed; status = UMI_STATUS_OK;
            /* XML attribute whitespace is normalised only for literal bytes;
             * numeric character references retain their specified value. */
            if (scalar == 9U || scalar == 10U || scalar == 13U) scalar = ' ';
        }
        if (status != UMI_STATUS_OK) return status;
        status = CtestXmlAppendScalar(value, capacity, &used, scalar);
        if (status != UMI_STATUS_OK) return status;
    }
    if (cursor->position == cursor->length) return UMI_STATUS_PARSE_ERROR;
    ++cursor->position; value[used] = '\0';
    return UMI_STATUS_OK;
}

static const char *CtestXmlAttribute(const UmiCtestXmlTag *tag, const char *name)
{
    for (size_t index = 0U; index < tag->count; ++index)
        if (strcmp(tag->keys[index], name) == 0) return tag->values[index];
    return NULL;
}

static UmiStatus CtestXmlReadTag(UmiCtestXmlCursor *cursor, UmiCtestXmlTag *tag)
{
    memset(tag, 0, sizeof *tag);
    if (!CtestXmlStarts(cursor, "<")) return UMI_STATUS_PARSE_ERROR;
    ++cursor->position;
    if (CtestXmlStarts(cursor, "/")) { ++cursor->position; tag->closing = 1; }
    UmiStatus status = CtestXmlName(cursor, tag->name, sizeof tag->name);
    if (status != UMI_STATUS_OK) return status;
    for (;;) {
        size_t beforeSpace = cursor->position;
        CtestXmlSkipSpace(cursor);
        if (CtestXmlStarts(cursor, ">")) { ++cursor->position; return UMI_STATUS_OK; }
        if (!tag->closing && CtestXmlStarts(cursor, "/>")) {
            cursor->position += 2U; tag->empty = 1; return UMI_STATUS_OK;
        }
        if (tag->closing || beforeSpace == cursor->position)
            return UMI_STATUS_PARSE_ERROR;
        if (tag->count == 24U) return UMI_STATUS_CAPACITY_EXCEEDED;
        status = CtestXmlName(cursor, tag->keys[tag->count], sizeof tag->keys[0]);
        if (status != UMI_STATUS_OK) return status;
        for (size_t index = 0U; index < tag->count; ++index)
            if (strcmp(tag->keys[index], tag->keys[tag->count]) == 0)
                return UMI_STATUS_PARSE_ERROR;
        CtestXmlSkipSpace(cursor);
        if (!CtestXmlStarts(cursor, "=")) return UMI_STATUS_PARSE_ERROR;
        ++cursor->position; CtestXmlSkipSpace(cursor);
        status = CtestXmlValue(cursor, tag->values[tag->count], sizeof tag->values[0]);
        if (status != UMI_STATUS_OK) return status;
        ++tag->count;
    }
}

static int CtestXmlUnsigned(const char *text, size_t *out)
{
    size_t value = 0U;
    if (text == NULL || text[0] == '\0') return 0;
    for (size_t index = 0U; text[index] != '\0'; ++index) {
        if (text[index] < '0' || text[index] > '9') return 0;
        size_t digit = (size_t)(text[index] - '0');
        if (value > (SIZE_MAX - digit) / 10U) return 0;
        value = value * 10U + digit;
    }
    *out = value; return 1;
}

/* Convert decimal seconds to integer milliseconds using decimal positions,
 * not binary floating point or the application's global locale. Fractional
 * milliseconds are discarded exactly, including scientific-notation timings. */
static int CtestXmlMilliseconds(const char *text, uint64_t *out)
{
    if (text == NULL || text[0] == '\0') return 0;
    char digits[1024]; size_t count = 0U, beforePoint = 0U;
    const char *cursor = text;
    while (*cursor >= '0' && *cursor <= '9') {
        if (count == sizeof digits) return 0;
        digits[count++] = *cursor++; ++beforePoint;
    }
    if (*cursor == '.') {
        ++cursor;
        while (*cursor >= '0' && *cursor <= '9') {
            if (count == sizeof digits) return 0;
            digits[count++] = *cursor++;
        }
    }
    if (count == 0U) return 0;
    int exponent = 0;
    if (*cursor == 'e' || *cursor == 'E') {
        ++cursor; int negative = 0, exponentDigits = 0;
        if (*cursor == '+' || *cursor == '-') negative = *cursor++ == '-';
        while (*cursor >= '0' && *cursor <= '9') {
            exponent = exponent * 10 + (*cursor++ - '0');
            if (exponent > 300) return 0;
            exponentDigits = 1;
        }
        if (!exponentDigits) return 0;
        if (negative) exponent = -exponent;
    }
    if (*cursor != '\0') return 0;
    int positions = (int)beforePoint + exponent + 3;
    uint64_t value = 0U;
    for (int index = 0; index < positions; ++index) {
        uint64_t digit = (size_t)index < count ? (uint64_t)(digits[index] - '0') : 0U;
        if (value > (UINT64_MAX - digit) / 10U) return 0;
        value = value * 10U + digit;
    }
    *out = value; return 1;
}

/* The declaration is a small grammar of its own: it must not silently accept
 * arbitrary processing instructions, misleading encodings or entity expansion. */
static UmiStatus CtestXmlDeclaration(UmiCtestXmlCursor *cursor, UmiCtestXmlTag *tag)
{
    cursor->position += 5U; /* The caller verified "<?xml". */
    memset(tag, 0, sizeof *tag);
    for (;;) {
        size_t beforeSpace = cursor->position;
        CtestXmlSkipSpace(cursor);
        if (CtestXmlStarts(cursor, "?>")) {
            if (tag->count == 0U) return UMI_STATUS_PARSE_ERROR;
            cursor->position += 2U; return UMI_STATUS_OK;
        }
        if (beforeSpace == cursor->position || tag->count == 3U)
            return UMI_STATUS_PARSE_ERROR;
        UmiStatus status = CtestXmlName(cursor, tag->keys[tag->count], sizeof tag->keys[0]);
        if (status != UMI_STATUS_OK) return status;
        for (size_t index = 0U; index < tag->count; ++index)
            if (strcmp(tag->keys[index], tag->keys[tag->count]) == 0)
                return UMI_STATUS_PARSE_ERROR;
        CtestXmlSkipSpace(cursor);
        if (!CtestXmlStarts(cursor, "=")) return UMI_STATUS_PARSE_ERROR;
        ++cursor->position; CtestXmlSkipSpace(cursor);
        size_t valueStart = cursor->position;
        status = CtestXmlValue(cursor, tag->values[tag->count], sizeof tag->values[0]);
        if (status != UMI_STATUS_OK) return status;
        if (memchr(cursor->bytes + valueStart, '&', cursor->position - valueStart) != NULL)
            return UMI_STATUS_PARSE_ERROR;
        const char *key = tag->keys[tag->count], *value = tag->values[tag->count];
        if (tag->count == 0U) {
            if (strcmp(key, "version") != 0 || strcmp(value, "1.0") != 0)
                return UMI_STATUS_PARSE_ERROR;
        } else if (strcmp(key, "encoding") == 0) {
            if (tag->count != 1U || (strcmp(value, "UTF-8") != 0 && strcmp(value, "utf-8") != 0))
                return UMI_STATUS_PARSE_ERROR;
        } else if (strcmp(key, "standalone") == 0) {
            if (strcmp(value, "yes") != 0 && strcmp(value, "no") != 0)
                return UMI_STATUS_PARSE_ERROR;
        } else return UMI_STATUS_PARSE_ERROR;
        ++tag->count;
    }
}

/* CDATA and comments contain literal text, not structural markup. Their UTF-8
 * and XML characters still have to be valid before the report can be trusted. */
static UmiStatus CtestXmlLiteralUntil(UmiCtestXmlCursor *cursor, const char *end)
{
    while (cursor->position < cursor->length && !CtestXmlStarts(cursor, end)) {
        size_t used; uint32_t value;
        if (!CtestUtf8Read(cursor->bytes + cursor->position,
            cursor->length - cursor->position, &used, &value))
            return UMI_STATUS_PARSE_ERROR;
        cursor->position += used;
    }
    return cursor->position < cursor->length ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

static int CtestXmlTextParent(const char *parent)
{
    return strcmp(parent, "system-out") == 0 || strcmp(parent, "system-err") == 0 ||
        strcmp(parent, "failure") == 0 || strcmp(parent, "skipped") == 0;
}

static int CtestXmlAllowed(const char *parent, const char *name)
{
    if (parent == NULL) return strcmp(name, "testsuite") == 0;
    if (strcmp(parent, "testsuite") == 0) return strcmp(name, "testcase") == 0;
    if (strcmp(parent, "testcase") == 0)
        return CtestXmlTextParent(name) || strcmp(name, "properties") == 0;
    return strcmp(parent, "properties") == 0 && strcmp(name, "property") == 0;
}

static UmiStatus CtestReportState(const char *status, int failures, int skips,
    const char *message, UmiCtestReportSnapshot *snapshot)
{
    if (strcmp(status, "run") == 0 && failures == 0 && skips == 0)
        snapshot->state = UMI_TEST_STATE_PASSED;
    else if (strcmp(status, "disabled") == 0 && failures == 0 && skips == 0)
        snapshot->state = UMI_TEST_STATE_SKIPPED;
    else if (strcmp(status, "fail") == 0 && failures == 1 && skips == 0)
        snapshot->state = strcmp(message, "Timeout") == 0
            ? UMI_TEST_STATE_TIMED_OUT : UMI_TEST_STATE_FAILED;
    else if (strcmp(status, "notrun") == 0 && failures == 0 && skips == 1)
        snapshot->state = strncmp(message, "SKIP_RETURN_CODE=", 17U) == 0 ||
            strcmp(message, "SKIP_REGULAR_EXPRESSION_MATCHED") == 0
            ? UMI_TEST_STATE_SKIPPED : UMI_TEST_STATE_NOT_RUN;
    else return UMI_STATUS_PARSE_ERROR;
    if (strcmp(status, "disabled") == 0)
        snprintf(snapshot->message, sizeof snapshot->message, "%s", "Test is disabled.");
    else snprintf(snapshot->message, sizeof snapshot->message, "%s", message);
    return UMI_STATUS_OK;
}

UmiStatus UmiCtestParseReport(const char *xml, size_t length,
    const char *testName, UmiCtestReportSnapshot *outSnapshot)
{
    size_t nameLength;
    UmiStatus status = CtestNameLength(testName, &nameLength);
    (void)nameLength;
    if (status != UMI_STATUS_OK) return status;
    if (xml == NULL || outSnapshot == NULL || length == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_CTEST_REPORT_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(xml, '\0', length) != NULL) return UMI_STATUS_PARSE_ERROR;
    UmiCtestXmlCursor cursor = { xml, length, 0U };
    UmiCtestXmlTag *tag = (UmiCtestXmlTag *)calloc(1U, sizeof *tag);
    if (tag == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    char stack[8][64]; size_t depth = 0U;
    size_t caseCount = 0U, declaredCount = 0U, matches = 0U;
    int sawRoot = 0, finishedRoot = 0, selected = 0, failures = 0, skips = 0;
    char caseStatus[32] = "", message[256] = "";
    UmiCtestReportSnapshot current = {0}, selectedSnapshot = {0};
#define CTEST_REPORT_FAIL(code) do { status = (code); goto cleanup; } while (0)
    if (CtestXmlStarts(&cursor, "\xEF\xBB\xBF")) cursor.position += 3U;
    /* Only a validated UTF-8 XML declaration may precede the root. */
    if (CtestXmlStarts(&cursor, "<?xml")) {
        status = CtestXmlDeclaration(&cursor, tag);
        if (status != UMI_STATUS_OK) goto cleanup;
    }
    while (cursor.position < length) {
        if (CtestXmlStarts(&cursor, "<!--")) {
            cursor.position += 4U;
            status = CtestXmlLiteralUntil(&cursor, "--");
            if (status != UMI_STATUS_OK) goto cleanup;
            if (!CtestXmlStarts(&cursor, "-->")) CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
            cursor.position += 3U; continue;
        }
        if (CtestXmlStarts(&cursor, "<![CDATA[")) {
            if (depth == 0U || !CtestXmlTextParent(stack[depth - 1U]))
                CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
            cursor.position += 9U;
            status = CtestXmlLiteralUntil(&cursor, "]]>");
            if (status != UMI_STATUS_OK) goto cleanup;
            if (cursor.position == length) CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
            cursor.position += 3U; continue;
        }
        if (xml[cursor.position] != '<') {
            if (CtestXmlStarts(&cursor, "]]>")) CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
            uint32_t value; size_t consumed;
            if (xml[cursor.position] == '&') {
                if (depth == 0U) CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
                status = CtestXmlEntity(&cursor, &value);
                if (status != UMI_STATUS_OK) goto cleanup;
            } else {
                if (!CtestUtf8Read(xml + cursor.position, length - cursor.position,
                                  &consumed, &value)) CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
                cursor.position += consumed;
            }
            if ((depth == 0U || !CtestXmlTextParent(stack[depth - 1U])) &&
                !(value <= 127U && CtestXmlSpace((unsigned char)value)))
                CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
            continue;
        }
        status = CtestXmlReadTag(&cursor, tag);
        if (status != UMI_STATUS_OK) goto cleanup;
        if (tag->closing) {
            if (depth == 0U || strcmp(stack[depth - 1U], tag->name) != 0)
                CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
        } else {
            if (finishedRoot || depth == 8U ||
                !CtestXmlAllowed(depth == 0U ? NULL : stack[depth - 1U], tag->name))
                CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
            if (strcmp(tag->name, "testsuite") == 0) {
                if (sawRoot || !CtestXmlUnsigned(CtestXmlAttribute(tag, "tests"), &declaredCount))
                    CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
                sawRoot = 1;
            } else if (strcmp(tag->name, "testcase") == 0) {
                const char *name = CtestXmlAttribute(tag, "name");
                const char *state = CtestXmlAttribute(tag, "status");
                const char *time = CtestXmlAttribute(tag, "time");
                if (name == NULL || name[0] == '\0' || state == NULL ||
                    strlen(state) >= sizeof caseStatus ||
                    !CtestXmlMilliseconds(time, &current.duration_ms))
                    CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
                snprintf(caseStatus, sizeof caseStatus, "%s", state);
                selected = strcmp(name, testName) == 0;
                if (selected && ++matches > 1U) CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
                ++caseCount; failures = 0; skips = 0; message[0] = '\0';
            } else if (strcmp(tag->name, "failure") == 0 || strcmp(tag->name, "skipped") == 0) {
                const char *reason = CtestXmlAttribute(tag, "message");
                if (strcmp(tag->name, "failure") == 0) ++failures; else ++skips;
                if (failures + skips > 1 || reason == NULL || strlen(reason) >= sizeof message)
                    CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
                snprintf(message, sizeof message, "%s", reason);
            }
            snprintf(stack[depth++], sizeof stack[0], "%s", tag->name);
        }
        if (tag->closing || tag->empty) {
            if (strcmp(tag->name, "testcase") == 0) {
                status = CtestReportState(caseStatus, failures, skips, message, &current);
                if (status != UMI_STATUS_OK) goto cleanup;
                if (selected) selectedSnapshot = current;
            }
            --depth;
            if (depth == 0U) finishedRoot = 1;
        }
    }
    if (!sawRoot || !finishedRoot || depth != 0U || caseCount != declaredCount)
        CTEST_REPORT_FAIL(UMI_STATUS_PARSE_ERROR);
    if (matches != 1U) CTEST_REPORT_FAIL(UMI_STATUS_NOT_FOUND);
    selectedSnapshot.testcase_count = caseCount;
    *outSnapshot = selectedSnapshot;
    status = UMI_STATUS_OK;
cleanup:
    free(tag);
    return status;
#undef CTEST_REPORT_FAIL
}

UmiStatus UmiCtestResultRequirePassed(const UmiTestResult *result)
{
    if (result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (result->status != UMI_STATUS_OK) return result->status;
    return result->state == UMI_TEST_STATE_PASSED && result->exit_code == 0
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
