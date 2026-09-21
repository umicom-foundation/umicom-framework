/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/ctest_json_metadata.inc
 * PURPOSE:
 *   Map validated CTest records into the existing copied metadata contracts.
 *   Private to ctest.c; display commands are never used as executable shell text.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct UmiCtestJsonArray {
    UmiCtestJsonSpan *values;
    size_t count;
    size_t capacity;
} UmiCtestJsonArray;

typedef struct UmiCtestSourceGraph {
    UmiCtestJsonArray files;
    UmiCtestJsonArray nodes;
} UmiCtestSourceGraph;

static UmiStatus UmiCtestJsonArrayRead(UmiCtestJsonSpan value, size_t maximum,
    UmiCtestJsonArray *out)
{
    UmiCtestJsonReader r = {value.begin, value.end, UMI_STATUS_OK};
    if (value.begin == NULL || !UmiCtestJsonTake(&r, '[')) return UMI_STATUS_PARSE_ERROR;
    for (;;) {
        UmiCtestJsonSpan element;
        UmiCtestJsonSpace(&r);
        if (r.cursor < r.end && *r.cursor == ']') return UMI_STATUS_OK;
        if (out->count >= maximum) return UMI_STATUS_CAPACITY_EXCEEDED;
        element.begin = r.cursor;
        if (!UmiCtestJsonValue(&r, 0U)) return r.status;
        element.end = r.cursor;
        if (out->count == out->capacity) {
            size_t capacity = out->capacity == 0U ? 16U : out->capacity * 2U;
            UmiCtestJsonSpan *values;
            if (capacity > maximum) capacity = maximum;
            if (capacity > SIZE_MAX / sizeof(*values)) return UMI_STATUS_CAPACITY_EXCEEDED;
            values = (UmiCtestJsonSpan *)realloc(out->values, capacity * sizeof(*values));
            if (values == NULL) return UMI_STATUS_OUT_OF_MEMORY;
            out->values = values; out->capacity = capacity;
        }
        out->values[out->count++] = element;
        UmiCtestJsonSpace(&r);
        if (r.cursor < r.end && *r.cursor == ']') return UMI_STATUS_OK;
        if (!UmiCtestJsonTake(&r, ',')) return r.status;
    }
}

/* Provide the join string array operation used by this module and its client applications. */
static UmiStatus UmiCtestJsonJoin(UmiCtestJsonSpan value, const char *separator,
    char *out, size_t capacity)
{
    UmiCtestJsonReader r = {value.begin, value.end, UMI_STATUS_OK};
    size_t used = 0U, count = 0U;
    if (capacity == 0U || value.begin == NULL || !UmiCtestJsonTake(&r, '[')) return UMI_STATUS_PARSE_ERROR;
    out[0] = '\0';
    for (;;) {
        char text[1024]; size_t length, separatorLength;
        UmiCtestJsonSpace(&r);
        if (r.cursor < r.end && *r.cursor == ']') return UMI_STATUS_OK;
        if (!UmiCtestJsonString(&r, text, sizeof(text))) return r.status;
        length = strlen(text); separatorLength = count == 0U ? 0U : strlen(separator);
        if (separatorLength >= capacity - used || length >= capacity - used - separatorLength)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        if (separatorLength != 0U) { (void)memcpy(out + used, separator, separatorLength); used += separatorLength; }
        (void)memcpy(out + used, text, length + 1U); used += length; ++count;
        UmiCtestJsonSpace(&r);
        if (r.cursor < r.end && *r.cursor == ']') return UMI_STATUS_OK;
        if (!UmiCtestJsonTake(&r, ',')) return r.status;
    }
}

/* Parse timeout ms without changing the application's locale or relying on
 * locale-dependent strtod. Reject values that cannot fit the public uint32_t
 * millisecond field. Positive fractions round up rather than becoming no limit. */
static UmiStatus UmiCtestTimeout(UmiCtestJsonSpan value, uint32_t *out)
{
    char digits[128];
    size_t count = 0U, firstNonzero, index;
    int beforeDot = 0, fractional = 0, exponent = 0, negativeExponent = 0, integerDigits;
    uint32_t number = 0U;
    const char *p = value.begin;
    if (p == NULL || p == value.end || *p == '-') return UMI_STATUS_PARSE_ERROR;
    if ((size_t)(value.end - p) > sizeof(digits)) return UMI_STATUS_CAPACITY_EXCEEDED;
    while (p < value.end && *p != 'e' && *p != 'E') {
        if (*p == '.') fractional = 1;
        else if (*p >= '0' && *p <= '9') {
            digits[count++] = *p;
            if (!fractional) ++beforeDot;
        } else return UMI_STATUS_PARSE_ERROR;
        ++p;
    }
    if (p < value.end) {
        ++p;
        if (p < value.end && (*p == '+' || *p == '-')) { negativeExponent = *p == '-'; ++p; }
        while (p < value.end) {
            if (*p < '0' || *p > '9') return UMI_STATUS_PARSE_ERROR;
            if (exponent < 100000) exponent = exponent * 10 + (*p - '0');
            ++p;
        }
    }
    for (firstNonzero = 0U; firstNonzero < count && digits[firstNonzero] == '0'; ++firstNonzero) {}
    if (firstNonzero == count) { *out = 0U; return UMI_STATUS_OK; }
    integerDigits = beforeDot + (negativeExponent ? -exponent : exponent) + 3;
    if (integerDigits <= (int)firstNonzero) { *out = 1U; return UMI_STATUS_OK; }
    if (integerDigits - (int)firstNonzero > 10) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = firstNonzero; index < (size_t)integerDigits; ++index) {
        unsigned digit = index < count ? (unsigned)(digits[index] - '0') : 0U;
        if (number > (UINT32_MAX - digit) / 10U) return UMI_STATUS_CAPACITY_EXCEEDED;
        number = number * 10U + digit;
    }
    for (index = (size_t)integerDigits; index < count; ++index) {
        if (digits[index] != '0') {
            if (number == UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
            ++number; break;
        }
    }
    *out = number; return UMI_STATUS_OK;
}

static UmiStatus UmiCtestOptionsValidate(const UmiTestPlatformCtestImportOptions *options)
{
    if (options == NULL) return UMI_STATUS_INVALID_ARGUMENT;
#define UMI_CTEST_OPTION(field) \
    if (memchr(options->field, '\0', sizeof(options->field)) == NULL) return UMI_STATUS_CAPACITY_EXCEEDED
    UMI_CTEST_OPTION(project_id); UMI_CTEST_OPTION(suite_id);
    UMI_CTEST_OPTION(configuration); UMI_CTEST_OPTION(build_directory);
#undef UMI_CTEST_OPTION
    if (options->project_id[0] == '\0' || options->suite_id[0] == '\0' || options->build_directory[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(options->suite_id) > 127U - (sizeof("discovery.") - 1U))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/* Short identifiers keep their existing spelling. For a long (but representable)
 * name use two bounded hashes; every collision is checked against suite and name
 * in the staged registry before insert. Hashes are identifiers, not signatures. */
static uint64_t UmiCtestIdentityHash(const char *text)
{
    uint64_t hash = UINT64_C(14695981039346656037);
    while (*text != '\0') { hash ^= (unsigned char)*text++; hash *= UINT64_C(1099511628211); }
    return hash;
}

static UmiStatus UmiCtestProperties(UmiCtestJsonSpan test, UmiTestPlatformItemSnapshot *item)
{
    UmiCtestJsonSpan properties; UmiStatus status;
    unsigned seen = 0U;
    status = UmiCtestJsonField(test, "properties", &properties);
    if (status != UMI_STATUS_OK || properties.begin == NULL) return status;
    UmiCtestJsonReader r = {properties.begin, properties.end, UMI_STATUS_OK};
    if (!UmiCtestJsonTake(&r, '[')) return r.status;
    for (;;) {
        UmiCtestJsonSpan property, name, value; char key[256]; unsigned bit = 0U;
        UmiCtestJsonSpace(&r);
        if (r.cursor < r.end && *r.cursor == ']') return UMI_STATUS_OK;
        property.begin = r.cursor;
        if (!UmiCtestJsonValue(&r, 0U)) return r.status;
        property.end = r.cursor;
        status = UmiCtestJsonField(property, "name", &name);
        if (status == UMI_STATUS_OK) status = UmiCtestJsonText(name, key, sizeof(key));
        if (status == UMI_STATUS_OK) status = UmiCtestJsonField(property, "value", &value);
        if (status != UMI_STATUS_OK) return status;
        if (key[0] == '\0' || value.begin == NULL) return UMI_STATUS_PARSE_ERROR;
        if (strcmp(key, "LABELS") == 0) { bit = 1U; status = UmiCtestJsonJoin(value, ";", item->labels, sizeof(item->labels)); }
        else if (strcmp(key, "DISABLED") == 0) {
            bit = 2U;
            if (!UmiCtestJsonIs(value, "true") && !UmiCtestJsonIs(value, "false")) return UMI_STATUS_PARSE_ERROR;
            item->enabled = UmiCtestJsonIs(value, "false");
        } else if (strcmp(key, "TIMEOUT") == 0) { bit = 4U; status = UmiCtestTimeout(value, &item->timeout_ms); }
        else if (strcmp(key, "WORKING_DIRECTORY") == 0) {
            bit = 8U; status = UmiCtestJsonText(value, item->working_directory, sizeof(item->working_directory));
        }
        if (status != UMI_STATUS_OK) return status;
        if ((seen & bit) != 0U) return UMI_STATUS_PARSE_ERROR;
        seen |= bit;
        UmiCtestJsonSpace(&r);
        if (r.cursor < r.end && *r.cursor == ']') return UMI_STATUS_OK;
        if (!UmiCtestJsonTake(&r, ',')) return r.status;
    }
}

static UmiStatus UmiCtestSourceLocation(UmiCtestJsonSpan test, const UmiCtestSourceGraph *graph,
    UmiTestPlatformItemSnapshot *item)
{
    UmiCtestJsonSpan backtrace, file, line; size_t index, fileIndex, lineNumber;
    UmiStatus status = UmiCtestJsonField(test, "backtrace", &backtrace);
    if (status != UMI_STATUS_OK || backtrace.begin == NULL) return status;
    if (graph->nodes.count == 0U || graph->files.count == 0U) return UMI_STATUS_PARSE_ERROR;
    status = UmiCtestJsonUnsigned(backtrace, graph->nodes.count - 1U, &index);
    if (status == UMI_STATUS_OK) status = UmiCtestJsonField(graph->nodes.values[index], "file", &file);
    if (status == UMI_STATUS_OK) status = UmiCtestJsonUnsigned(file, graph->files.count - 1U, &fileIndex);
    if (status == UMI_STATUS_OK) status = UmiCtestJsonText(graph->files.values[fileIndex], item->source_uri, sizeof(item->source_uri));
    if (status == UMI_STATUS_OK) status = UmiCtestJsonField(graph->nodes.values[index], "line", &line);
    if (status != UMI_STATUS_OK) return status;
    if (line.begin != NULL) {
        status = UmiCtestJsonUnsigned(line, UINT32_MAX, &lineNumber);
        if (status != UMI_STATUS_OK) return status;
        if (lineNumber == 0U) return UMI_STATUS_PARSE_ERROR;
        item->source_line = (uint32_t)lineNumber;
    }
    return UMI_STATUS_OK;
}

/* Provide the import test object operation used by this module and its client applications. */
static UmiStatus UmiCtestImportTest(UmiCtestJsonSpan test,
    const UmiTestPlatformCtestImportOptions *options, const UmiCtestSourceGraph *graph,
    UmiTestPlatformItemRegistry *items, UmiTestPlatformCtestImportSummary *summary)
{
    UmiTestPlatformItemSnapshot item, previous;
    UmiCtestJsonSpan name, command, config;
    UmiStatus status;
    char configuration[128]; int written;
    (void)memset(&item, 0, sizeof(item));
    status = UmiCtestJsonField(test, "name", &name);
    if (status == UMI_STATUS_OK) status = UmiCtestJsonText(name, item.name, sizeof(item.name));
    if (status != UMI_STATUS_OK) return status;
    if (item.name[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    if (strlen(options->suite_id) + 1U + strlen(item.name) < sizeof(item.id)) {
        size_t prefixLength = strlen(options->suite_id);
        (void)memcpy(item.id, options->suite_id, prefixLength);
        item.id[prefixLength] = '.';
        (void)memcpy(item.id + prefixLength + 1U, item.name, strlen(item.name) + 1U);
    } else {
        written = snprintf(item.id, sizeof(item.id), "ctest.%016" PRIx64 ".%016" PRIx64,
            UmiCtestIdentityHash(options->suite_id), UmiCtestIdentityHash(item.name));
        if (written < 0 || (size_t)written >= sizeof(item.id)) return UMI_STATUS_INTERNAL_ERROR;
    }
    /* The cleared suite cannot contain a legitimate prior record here. A match
     * is a duplicate test or an identity collision with another suite/root. */
    status = umi_test_platform_item_registry_find(items, item.id, &previous);
    if (status == UMI_STATUS_OK) return UMI_STATUS_ALREADY_EXISTS;
    if (status != UMI_STATUS_NOT_FOUND) return status;
    item.struct_size = (uint32_t)sizeof(item); item.api_version = UMI_TEST_PLATFORM_ITEM_API_VERSION;
    (void)memcpy(item.parent_id, options->suite_id, strlen(options->suite_id) + 1U);
    (void)memcpy(item.suite_id, options->suite_id, strlen(options->suite_id) + 1U);
    (void)memcpy(item.uri, options->build_directory, strlen(options->build_directory) + 1U);
    (void)memcpy(item.working_directory, options->build_directory, strlen(options->build_directory) + 1U);
    (void)memcpy(item.framework, "ctest", sizeof("ctest"));
    (void)memcpy(item.kind, "test", sizeof("test"));
    item.enabled = 1; item.discovered = 1;
    status = UmiCtestJsonField(test, "command", &command);
    if (status == UMI_STATUS_OK && command.begin != NULL)
        status = UmiCtestJsonJoin(command, " ", item.command, sizeof(item.command));
    if (status == UMI_STATUS_OK) status = UmiCtestProperties(test, &item);
    if (status == UMI_STATUS_OK) status = UmiCtestSourceLocation(test, graph, &item);
    if (status == UMI_STATUS_OK) status = UmiCtestJsonField(test, "config", &config);
    if (status == UMI_STATUS_OK && config.begin != NULL) {
        status = UmiCtestJsonText(config, configuration, sizeof(configuration));
        if (status == UMI_STATUS_OK && options->configuration[0] != '\0' &&
            strcmp(configuration, options->configuration) != 0) status = UMI_STATUS_INVALID_STATE;
    }
    if (status != UMI_STATUS_OK) return status;
    status = umi_test_platform_item_registry_upsert(items, &item);
    if (status != UMI_STATUS_OK) return status;
    ++summary->discovered_count;
    if (!item.enabled) ++summary->disabled_count;
    if (item.labels[0] != '\0') ++summary->labelled_count;
    if (item.timeout_ms != 0U) ++summary->timed_count;
    return UMI_STATUS_OK;
}
