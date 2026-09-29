/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/inventory_text.c
 * Purpose: Parse complete bounded build inventories without partial publication.
 * Author: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "inventory_internal.h"
#include "inventory_codec.h"
#include <stdlib.h>
#include <string.h>

static int Hex(unsigned char c)
{
    if (c >= '0' && c <= '9') return (int)(c - '0');
    if (c >= 'a' && c <= 'f') return (int)(c - 'a') + 10;
    if (c >= 'A' && c <= 'F') return (int)(c - 'A') + 10;
    return -1;
}

/* Decode in place only after fields have been separated. Hex encoding keeps
 * tabs, semicolons, Unicode and CMake generator expressions out of TSV syntax.
 * A decoded NUL is refused because public views use terminated C strings. */
static UmiStatus Decode(char *text)
{
    size_t length = strlen(text);
    if ((length & 1U) != 0U) return UMI_STATUS_PARSE_ERROR;
    if (length / 2U > UMI_RELEASE_INVENTORY_FIELD_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < length; i += 2U) {
        int high = Hex((unsigned char)text[i]);
        int low = Hex((unsigned char)text[i + 1U]);
        if (high < 0 || low < 0 || (high == 0 && low == 0))
            return UMI_STATUS_PARSE_ERROR;
        text[i / 2U] = (char)((unsigned)high * 16U + (unsigned)low);
    }
    text[length / 2U] = '\0';
    return UMI_STATUS_OK;
}

static size_t Fields(char *line, char **fields, size_t capacity)
{
    size_t count = 1U;
    fields[0] = line;
    for (char *p = line; *p != '\0'; ++p) {
        if (*p == '\t') {
            if (count == capacity) return 0U;
            *p = '\0';
            fields[count++] = p + 1;
        }
    }
    return count;
}

static bool GenerationValid(const char *text)
{
    if (strlen(text) != 32U) return false;
    for (size_t i = 0U; i < 32U; ++i)
        if (Hex((unsigned char)text[i]) < 0) return false;
    return true;
}

static bool StateValid(const UmiReleaseInventoryRecord *row)
{
    switch (row->kind) {
    case UMI_RELEASE_INVENTORY_HEADER:
        return (strcmp(row->state, "unassigned") == 0 && row->owner[0] == '\0') ||
            (strcmp(row->state, "declared") == 0 && row->owner[0] != '\0');
    case UMI_RELEASE_INVENTORY_TARGET:
        return strcmp(row->state, "configured") == 0 && row->owner[0] != '\0';
    case UMI_RELEASE_INVENTORY_SOURCE:
        return row->owner[0] != '\0' &&
            (strcmp(row->state, "present") == 0 ||
             strcmp(row->state, "unresolved") == 0);
    case UMI_RELEASE_INVENTORY_TEST:
        return strcmp(row->state, "registered") == 0 ||
            strcmp(row->state, "disabled") == 0 ||
            strcmp(row->state, "disabled-missing-command") == 0 ||
            strcmp(row->state, "missing-command") == 0;
    }
    return false;
}

static int CompareRecords(const void *left, const void *right)
{
    const UmiReleaseInventoryRecord *a = left, *b = right;
    if (a->kind != b->kind) return a->kind < b->kind ? -1 : 1;
    int identity = strcmp(a->identity, b->identity);
    return identity != 0 ? identity : strcmp(a->owner, b->owner);
}

UmiStatus UmiReleaseInventoryParse(const char *text, size_t length,
    UmiReleaseInventory **outInventory)
{
    if (text == NULL || outInventory == NULL || *outInventory != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_RELEASE_INVENTORY_TEXT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0U || memchr(text, '\0', length) != NULL)
        return UMI_STATUS_PARSE_ERROR;
    size_t lines = text[length - 1U] == '\n' ? 0U : 1U;
    for (size_t i = 0U; i < length; ++i) if (text[i] == '\n') ++lines;
    if (lines < 2U) return UMI_STATUS_PARSE_ERROR;
    if (lines - 2U > UMI_RELEASE_INVENTORY_RECORD_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiReleaseInventory *inventory = calloc(1U, sizeof(*inventory));
    if (inventory == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    inventory->storage = malloc(length + 1U);
    inventory->records = calloc(lines > 2U ? lines - 2U : 1U,
        sizeof(*inventory->records));
    if (inventory->storage == NULL || inventory->records == NULL) {
        UmiReleaseInventoryDestroy(inventory);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(inventory->storage, text, length);
    inventory->storage[length] = '\0';
    UmiStatus status = UMI_STATUS_OK;
    char *cursor = inventory->storage;
    for (size_t number = 0U; number < lines && status == UMI_STATUS_OK; ++number) {
        char *line = cursor;
        char *end = strchr(line, '\n');
        if (end != NULL) { *end = '\0'; cursor = end + 1; }
        else cursor = line + strlen(line);
        size_t size = strlen(line);
        if (size != 0U && line[size - 1U] == '\r') line[size - 1U] = '\0';
        if (number == 0U) {
            if (strcmp(line, "UMICOM-RELEASE-INVENTORY\t1") != 0)
                status = UMI_STATUS_PARSE_ERROR;
            continue;
        }
        char *fields[6];
        size_t count = Fields(line, fields, 6U);
        if (number == 1U) {
            if (count != 6U || strcmp(fields[0], "context") != 0 ||
                (strcmp(fields[1], "cmake") != 0 && strcmp(fields[1], "ctest") != 0) ||
                !GenerationValid(fields[2])) { status = UMI_STATUS_PARSE_ERROR; continue; }
            inventory->producer = fields[1]; inventory->generation = fields[2];
            inventory->sourceRoot = fields[3]; inventory->buildRoot = fields[4];
            inventory->configuration = fields[5];
            for (size_t i = 3U; i < 6U && status == UMI_STATUS_OK; ++i)
                status = Decode(fields[i]);
            if (status == UMI_STATUS_OK &&
                (fields[3][0] == '\0' || fields[4][0] == '\0' || fields[5][0] == '\0'))
                status = UMI_STATUS_PARSE_ERROR;
            continue;
        }
        if (count != 5U) { status = UMI_STATUS_PARSE_ERROR; continue; }
        UmiReleaseInventoryRecord *row = &inventory->records[inventory->count];
        if (strcmp(fields[0], "header") == 0) row->kind = UMI_RELEASE_INVENTORY_HEADER;
        else if (strcmp(fields[0], "target") == 0) row->kind = UMI_RELEASE_INVENTORY_TARGET;
        else if (strcmp(fields[0], "source") == 0) row->kind = UMI_RELEASE_INVENTORY_SOURCE;
        else if (strcmp(fields[0], "test") == 0) row->kind = UMI_RELEASE_INVENTORY_TEST;
        else { status = UMI_STATUS_PARSE_ERROR; continue; }
        row->identity = fields[1]; row->owner = fields[2];
        row->state = fields[3]; row->detail = fields[4];
        status = Decode(fields[1]);
        if (status == UMI_STATUS_OK) status = Decode(fields[2]);
        if (status == UMI_STATUS_OK) status = Decode(fields[4]);
        if (status == UMI_STATUS_OK && (row->identity[0] == '\0' || !StateValid(row)))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK && strcmp(inventory->producer, "ctest") == 0 &&
            row->kind != UMI_RELEASE_INVENTORY_TEST) status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK && strcmp(inventory->producer, "cmake") == 0 &&
            row->kind == UMI_RELEASE_INVENTORY_TEST && strcmp(row->state, "registered") != 0)
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK) ++inventory->count;
    }
    if (status == UMI_STATUS_OK) {
        qsort(inventory->records, inventory->count, sizeof(*inventory->records), CompareRecords);
        for (size_t i = 1U; i < inventory->count; ++i) {
            const UmiReleaseInventoryRecord *a = &inventory->records[i - 1U], *b = &inventory->records[i];
            if (a->kind == b->kind && strcmp(a->identity, b->identity) == 0 &&
                (a->kind != UMI_RELEASE_INVENTORY_SOURCE || strcmp(a->owner, b->owner) == 0)) {
                status = UMI_STATUS_ALREADY_EXISTS; break;
            }
        }
    }
    if (status != UMI_STATUS_OK) { UmiReleaseInventoryDestroy(inventory); return status; }
    *outInventory = inventory;
    return UMI_STATUS_OK;
}

/* Policy uses the same bounded field grammar. These internal entry points keep
 * the established inventory parser intact while avoiding a second hex codec. */
UmiStatus UmiReleaseInventoryDecodeField(char *text) { return Decode(text); }
size_t UmiReleaseInventorySplitFields(char *line, char **fields, size_t capacity)
{ return Fields(line, fields, capacity); }
bool UmiReleaseInventoryGenerationValid(const char *text) { return GenerationValid(text); }
