/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/ctest_commands.inc
 *
 * PURPOSE:
 *   Share strict discovery parsing with executable ownership checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Included by ctest.c after its private reader. Sharing those readers preserves
 * Unicode, duplicate-field and document-boundary rules across both consumers. */
typedef struct UmiCtestCommandEntry {
    char *name;
    char *program;
} UmiCtestCommandEntry;
struct UmiCtestCommandCatalogue {
    UmiCtestCommandEntry *entries;
    size_t count;
};
void UmiCtestCommandCatalogueDestroy(UmiCtestCommandCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    for (size_t i = 0U; i < catalogue->count; ++i) {
        free(catalogue->entries[i].name);
        free(catalogue->entries[i].program);
    }
    free(catalogue->entries);
    free(catalogue);
}
size_t UmiCtestCommandCatalogueCount(const UmiCtestCommandCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->count;
}
static int UmiCtestCommandCompare(const void *left, const void *right)
{
    const UmiCtestCommandEntry *a = left;
    const UmiCtestCommandEntry *b = right;
    return strcmp(a->name, b->name);
}
static UmiStatus UmiCtestCommandCopy(UmiCtestJsonSpan span, size_t maximum, char **out)
{
    if (span.begin == NULL) return UMI_STATUS_PARSE_ERROR;
    size_t capacity = (size_t)(span.end - span.begin) + 1U;
    if (capacity > maximum + 1U) capacity = maximum + 1U;
    char *text = malloc(capacity);
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiCtestJsonText(span, text, capacity);
    if (status == UMI_STATUS_OK && text[0] == '\0') status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK) { free(text); return status; }
    *out = text;
    return UMI_STATUS_OK;
}
UmiStatus UmiCtestCommandCatalogueParse(const char *json, size_t length,
    UmiCtestCommandCatalogue **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (json == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_TEST_PLATFORM_CTEST_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0U || memchr(json, '\0', length) != NULL) return UMI_STATUS_PARSE_ERROR;
    UmiCtestJsonReader reader = {json, json + length, UMI_STATUS_OK};
    if (!UmiCtestJsonValue(&reader, 0U)) return reader.status;
    UmiCtestJsonSpace(&reader);
    if (reader.cursor != reader.end) return UMI_STATUS_PARSE_ERROR;
    UmiCtestCommandCatalogue *result = calloc(1U, sizeof(*result));
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiCtestJsonArray tests = {NULL, 0U, 0U};
    UmiCtestJsonArray command = {NULL, 0U, 0U};
    UmiCtestJsonSpan document = {json, json + length}, value, version;
    UmiStatus status = UMI_STATUS_OK;
    size_t number = 0U;
    char kind[32];
#define UMI_COMMAND_TRY(call) do { status = (call); if (status != UMI_STATUS_OK) goto cleanup; } while (0)
    UMI_COMMAND_TRY(UmiCtestJsonField(document, "kind", &value));
    UMI_COMMAND_TRY(UmiCtestJsonText(value, kind, sizeof(kind)));
    if (strcmp(kind, "ctestInfo") != 0) { status = UMI_STATUS_PARSE_ERROR; goto cleanup; }
    UMI_COMMAND_TRY(UmiCtestJsonField(document, "version", &version));
    UMI_COMMAND_TRY(UmiCtestJsonField(version, "major", &value));
    UMI_COMMAND_TRY(UmiCtestJsonUnsigned(value, UINT32_MAX, &number));
    if (number != 1U) { status = UMI_STATUS_NOT_IMPLEMENTED; goto cleanup; }
    UMI_COMMAND_TRY(UmiCtestJsonField(version, "minor", &value));
    UMI_COMMAND_TRY(UmiCtestJsonUnsigned(value, UINT32_MAX, &number));
    if (number != 0U) { status = UMI_STATUS_NOT_IMPLEMENTED; goto cleanup; }
    UMI_COMMAND_TRY(UmiCtestJsonField(document, "tests", &value));
    UMI_COMMAND_TRY(UmiCtestJsonArrayRead(value, 65536U, &tests));
    if (tests.count != 0U) {
        result->entries = calloc(tests.count, sizeof(*result->entries));
        if (result->entries == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    }
    result->count = tests.count;
    for (size_t i = 0U; i < tests.count; ++i) {
        UMI_COMMAND_TRY(UmiCtestJsonField(tests.values[i], "name", &value));
        UMI_COMMAND_TRY(UmiCtestCommandCopy(value, 4095U, &result->entries[i].name));
        UMI_COMMAND_TRY(UmiCtestJsonField(tests.values[i], "command", &value));
        if (value.begin == NULL) continue;
        command.count = 0U;
        UMI_COMMAND_TRY(UmiCtestJsonArrayRead(value, 65536U, &command));
        for (size_t argument = 0U; argument < command.count; ++argument) {
            /* Even arguments that do not affect the owner path must retain
             * CTest's string-array shape. The document reader validates UTF-8. */
            if (*command.values[argument].begin != '"') {
                status = UMI_STATUS_PARSE_ERROR; goto cleanup;
            }
        }
        if (command.count != 0U)
            UMI_COMMAND_TRY(UmiCtestCommandCopy(command.values[0], 32767U,
                &result->entries[i].program));
    }
    if (result->count > 1U) {
        qsort(result->entries, result->count, sizeof(*result->entries), UmiCtestCommandCompare);
        for (size_t i = 1U; i < result->count; ++i)
            if (strcmp(result->entries[i - 1U].name, result->entries[i].name) == 0) {
                status = UMI_STATUS_ALREADY_EXISTS; goto cleanup;
            }
    }
    *out = result;
    result = NULL;
cleanup:
    free(command.values);
    free(tests.values);
    UmiCtestCommandCatalogueDestroy(result);
    return status;
#undef UMI_COMMAND_TRY
}
UmiStatus UmiCtestCommandCatalogueFind(const UmiCtestCommandCatalogue *catalogue,
    const char *name, const char **outProgram)
{
    if (catalogue == NULL || name == NULL || outProgram == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t low = 0U, high = catalogue->count;
    while (low < high) {
        size_t middle = low + (high - low) / 2U;
        int order = strcmp(name, catalogue->entries[middle].name);
        if (order < 0) high = middle;
        else if (order > 0) low = middle + 1U;
        else {
            if (catalogue->entries[middle].program == NULL) return UMI_STATUS_NOT_FOUND;
            *outProgram = catalogue->entries[middle].program;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
