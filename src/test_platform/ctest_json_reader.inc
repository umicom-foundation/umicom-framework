/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/ctest_json_reader.inc
 * PURPOSE:
 *   Validate complete, bounded CTest JSON and traverse borrowed value spans.
 *   Replaces the provider's permissive substring scanner; no public parser or
 *   second application data model is introduced. Compiled only through ctest.c.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct UmiCtestJsonSpan { const char *begin; const char *end; } UmiCtestJsonSpan;
typedef struct UmiCtestJsonReader { const char *cursor; const char *end; UmiStatus status; } UmiCtestJsonReader;

static void UmiCtestJsonSpace(UmiCtestJsonReader *reader)
{
    while (reader->cursor < reader->end && (*reader->cursor == ' ' ||
        *reader->cursor == '\t' || *reader->cursor == '\r' || *reader->cursor == '\n')) ++reader->cursor;
}

static int UmiCtestJsonTake(UmiCtestJsonReader *reader, char expected)
{
    UmiCtestJsonSpace(reader);
    if (reader->cursor == reader->end || *reader->cursor != expected) {
        reader->status = UMI_STATUS_PARSE_ERROR; return 0;
    }
    ++reader->cursor; return 1;
}

static unsigned UmiCtestJsonHex(char c)
{
    if (c >= '0' && c <= '9') return (unsigned)(c - '0');
    if (c >= 'a' && c <= 'f') return (unsigned)(c - 'a') + 10U;
    if (c >= 'A' && c <= 'F') return (unsigned)(c - 'A') + 10U;
    return 16U;
}

static int UmiCtestJsonHexWord(UmiCtestJsonReader *reader, uint32_t *out)
{
    size_t index; uint32_t value = 0U;
    if ((size_t)(reader->end - reader->cursor) < 4U) return 0;
    for (index = 0U; index < 4U; ++index) {
        unsigned digit = UmiCtestJsonHex(*reader->cursor++);
        if (digit > 15U) return 0;
        value = value * 16U + digit;
    }
    *out = value; return 1;
}

/* Decode exactly one JSON string, validating escaped and raw Unicode. A NULL
 * destination requests validation only, so unknown property text is still checked. */
static int UmiCtestJsonString(UmiCtestJsonReader *reader, char *out, size_t capacity)
{
    size_t used = 0U;
    if (!UmiCtestJsonTake(reader, '"')) return 0;
    while (reader->cursor < reader->end) {
        unsigned char bytes[4]; size_t length = 0U, index;
        uint32_t cp; unsigned char c = (unsigned char)*reader->cursor++;
        if (c == '"') {
            if (out != NULL) {
                if (used >= capacity) { reader->status = UMI_STATUS_CAPACITY_EXCEEDED; return 0; }
                out[used] = '\0';
            }
            return 1;
        }
        if (c < 0x20U) break;
        if (c == '\\') {
            if (reader->cursor == reader->end) break;
            c = (unsigned char)*reader->cursor++;
            if (c == 'u') {
                if (!UmiCtestJsonHexWord(reader, &cp)) break;
                if (cp >= 0xd800U && cp <= 0xdbffU) {
                    uint32_t low;
                    if ((size_t)(reader->end - reader->cursor) < 6U ||
                        reader->cursor[0] != '\\' || reader->cursor[1] != 'u') break;
                    reader->cursor += 2;
                    if (!UmiCtestJsonHexWord(reader, &low) || low < 0xdc00U || low > 0xdfffU) break;
                    cp = 0x10000U + (cp - 0xd800U) * 1024U + low - 0xdc00U;
                } else if (cp >= 0xdc00U && cp <= 0xdfffU) break;
                if (cp <= 0x7fU) { bytes[0] = (unsigned char)cp; length = 1U; }
                else if (cp <= 0x7ffU) {
                    bytes[0] = (unsigned char)(0xc0U | (cp >> 6U));
                    bytes[1] = (unsigned char)(0x80U | (cp & 63U)); length = 2U;
                } else if (cp <= 0xffffU) {
                    bytes[0] = (unsigned char)(0xe0U | (cp >> 12U));
                    bytes[1] = (unsigned char)(0x80U | ((cp >> 6U) & 63U));
                    bytes[2] = (unsigned char)(0x80U | (cp & 63U)); length = 3U;
                } else {
                    bytes[0] = (unsigned char)(0xf0U | (cp >> 18U));
                    bytes[1] = (unsigned char)(0x80U | ((cp >> 12U) & 63U));
                    bytes[2] = (unsigned char)(0x80U | ((cp >> 6U) & 63U));
                    bytes[3] = (unsigned char)(0x80U | (cp & 63U)); length = 4U;
                }
            } else {
                switch (c) {
                    case '"': case '\\': case '/': bytes[0] = c; break;
                    case 'b': bytes[0] = '\b'; break;
                    case 'f': bytes[0] = '\f'; break;
                    case 'n': bytes[0] = '\n'; break;
                    case 'r': bytes[0] = '\r'; break;
                    case 't': bytes[0] = '\t'; break;
                    default: reader->status = UMI_STATUS_PARSE_ERROR; return 0;
                }
                length = 1U;
            }
        } else if (c < 0x80U) { bytes[0] = c; length = 1U; }
        else {
            uint32_t minimum;
            bytes[0] = c;
            if (c >= 0xc2U && c <= 0xdfU) { length = 2U; cp = c & 31U; minimum = 0x80U; }
            else if (c >= 0xe0U && c <= 0xefU) { length = 3U; cp = c & 15U; minimum = 0x800U; }
            else if (c >= 0xf0U && c <= 0xf4U) { length = 4U; cp = c & 7U; minimum = 0x10000U; }
            else break;
            if ((size_t)(reader->end - reader->cursor) < length - 1U) break;
            for (index = 1U; index < length; ++index) {
                bytes[index] = (unsigned char)*reader->cursor++;
                if ((bytes[index] & 0xc0U) != 0x80U) { reader->status = UMI_STATUS_PARSE_ERROR; return 0; }
                cp = cp * 64U + (bytes[index] & 63U);
            }
            if (cp < minimum || cp > 0x10ffffU || (cp >= 0xd800U && cp <= 0xdfffU)) break;
        }
        if (out != NULL) {
            if (length == 1U && bytes[0] == 0U) { reader->status = UMI_STATUS_PARSE_ERROR; return 0; }
            if (used >= capacity || length >= capacity - used) {
                reader->status = UMI_STATUS_CAPACITY_EXCEEDED; return 0;
            }
            (void)memcpy(out + used, bytes, length); used += length;
        }
    }
    reader->status = UMI_STATUS_PARSE_ERROR; return 0;
}

static int UmiCtestJsonNumber(UmiCtestJsonReader *reader)
{
    const char *p = reader->cursor;
    if (p < reader->end && *p == '-') ++p;
    if (p == reader->end) return 0;
    if (*p == '0') ++p;
    else {
        if (*p < '1' || *p > '9') return 0;
        do { ++p; } while (p < reader->end && *p >= '0' && *p <= '9');
    }
    if (p < reader->end && *p == '.') {
        ++p;
        if (p == reader->end || *p < '0' || *p > '9') return 0;
        do { ++p; } while (p < reader->end && *p >= '0' && *p <= '9');
    }
    if (p < reader->end && (*p == 'e' || *p == 'E')) {
        ++p; if (p < reader->end && (*p == '+' || *p == '-')) ++p;
        if (p == reader->end || *p < '0' || *p > '9') return 0;
        do { ++p; } while (p < reader->end && *p >= '0' && *p <= '9');
    }
    reader->cursor = p; return 1;
}

static int UmiCtestJsonValue(UmiCtestJsonReader *reader, unsigned depth)
{
    char close;
    UmiCtestJsonSpace(reader);
    if (depth > 64U) { reader->status = UMI_STATUS_CAPACITY_EXCEEDED; return 0; }
    if (reader->cursor == reader->end) goto malformed;
    if (*reader->cursor == '"') return UmiCtestJsonString(reader, NULL, 0U);
    if (*reader->cursor == '[' || *reader->cursor == '{') {
        int object = *reader->cursor++ == '{'; close = object ? '}' : ']';
        UmiCtestJsonSpace(reader);
        if (reader->cursor < reader->end && *reader->cursor == close) { ++reader->cursor; return 1; }
        for (;;) {
            if (object && (!UmiCtestJsonString(reader, NULL, 0U) || !UmiCtestJsonTake(reader, ':'))) return 0;
            if (!UmiCtestJsonValue(reader, depth + 1U)) return 0;
            UmiCtestJsonSpace(reader);
            if (reader->cursor == reader->end) goto malformed;
            if (*reader->cursor == close) { ++reader->cursor; return 1; }
            if (*reader->cursor++ != ',') goto malformed;
            /* A value/key is mandatory after a comma; trailing commas fail. */
        }
    }
    if ((size_t)(reader->end - reader->cursor) >= 4U &&
        (memcmp(reader->cursor, "true", 4U) == 0 || memcmp(reader->cursor, "null", 4U) == 0)) {
        reader->cursor += 4; return 1;
    }
    if ((size_t)(reader->end - reader->cursor) >= 5U && memcmp(reader->cursor, "false", 5U) == 0) {
        reader->cursor += 5; return 1;
    }
    if (UmiCtestJsonNumber(reader)) return 1;
malformed:
    reader->status = UMI_STATUS_PARSE_ERROR; return 0;
}

/* The span was validated already. Still propagate errors when a field cannot
 * be represented. Duplicate requested keys are rejected, never first/last wins. */
static UmiStatus UmiCtestJsonField(UmiCtestJsonSpan object, const char *name,
    UmiCtestJsonSpan *out)
{
    UmiCtestJsonReader r = {object.begin, object.end, UMI_STATUS_OK};
    int found = 0;
    out->begin = NULL; out->end = NULL;
    if (object.begin == NULL) return UMI_STATUS_PARSE_ERROR;
    if (!UmiCtestJsonTake(&r, '{')) return r.status;
    for (;;) {
        char key[256]; UmiCtestJsonSpan value;
        UmiCtestJsonSpace(&r);
        if (r.cursor < r.end && *r.cursor == '}') return UMI_STATUS_OK;
        if (!UmiCtestJsonString(&r, key, sizeof(key)) || !UmiCtestJsonTake(&r, ':')) return r.status;
        UmiCtestJsonSpace(&r); value.begin = r.cursor;
        if (!UmiCtestJsonValue(&r, 0U)) return r.status;
        value.end = r.cursor;
        if (strcmp(key, name) == 0) {
            if (found) return UMI_STATUS_PARSE_ERROR;
            *out = value; found = 1;
        }
        UmiCtestJsonSpace(&r);
        if (r.cursor < r.end && *r.cursor == '}') return UMI_STATUS_OK;
        if (!UmiCtestJsonTake(&r, ',')) return r.status;
    }
}

static UmiStatus UmiCtestJsonText(UmiCtestJsonSpan value, char *out, size_t capacity)
{
    UmiCtestJsonReader r = {value.begin, value.end, UMI_STATUS_OK};
    if (value.begin == NULL) return UMI_STATUS_PARSE_ERROR;
    if (!UmiCtestJsonString(&r, out, capacity) || r.cursor != r.end) return r.status != UMI_STATUS_OK ? r.status : UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}

static UmiStatus UmiCtestJsonUnsigned(UmiCtestJsonSpan value, size_t maximum, size_t *out)
{
    size_t number = 0U; const char *p;
    if (value.begin == NULL || value.begin == value.end) return UMI_STATUS_PARSE_ERROR;
    for (p = value.begin; p < value.end; ++p) {
        unsigned digit;
        if (*p < '0' || *p > '9') return UMI_STATUS_PARSE_ERROR;
        digit = (unsigned)(*p - '0');
        if ((size_t)digit > maximum || number > (maximum - digit) / 10U) return UMI_STATUS_CAPACITY_EXCEEDED;
        number = number * 10U + digit;
    }
    *out = number; return UMI_STATUS_OK;
}

static int UmiCtestJsonIs(UmiCtestJsonSpan value, const char *literal)
{
    return value.begin != NULL && (size_t)(value.end - value.begin) == strlen(literal) &&
        memcmp(value.begin, literal, strlen(literal)) == 0;
}
