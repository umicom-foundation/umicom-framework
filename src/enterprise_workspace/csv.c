/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/enterprise_workspace/csv.c
 *
 * PURPOSE:
 *   Parse bounded stock CSV and freeze every proposed change before review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *const recipeIds[] = {"stock.csv", "stock.csv.upper"};
size_t EwsRecipeIndex(const char *id)
{
    if (id == NULL) return SIZE_MAX;
    for (size_t i = 0U; i < UMI_ENTERPRISE_RECIPE_COUNT; ++i)
        if (strcmp(id, recipeIds[i]) == 0) return i;
    return SIZE_MAX;
}
void EwsRecipe(size_t index, UmiEnterpriseRecipe *outRecipe)
{
    (void)memset(outRecipe, 0, sizeof(*outRecipe));
    (void)EwsCopy(outRecipe->id, sizeof(outRecipe->id), recipeIds[index]);
    (void)EwsCopy(outRecipe->name, sizeof(outRecipe->name),
        index == 0U ? "Stock CSV" : "Stock CSV with uppercase labels");
    (void)EwsCopy(outRecipe->description, sizeof(outRecipe->description),
        index == 0U ? "Import item_id, label and quantity exactly as reviewed. Built into Framework; no downloaded code."
                    : "Import the same schema and uppercase ASCII letters in labels. Other UTF-8 characters stay unchanged.");
    outRecipe->contractVersion = 1U;
}
static UmiStatus Fail(UmiEnterpriseIssue *issue, size_t record, size_t column,
    UmiStatus status, const char *message)
{
    if (issue != NULL) {
        issue->record = record; issue->column = column;
        (void)EwsCopy(issue->message, sizeof(issue->message), message);
    }
    return status;
}

/* Cursor advancement is explicit: a closing quote accepts only a delimiter or
 * record end. This prevents trailing text from silently changing a field. */
static UmiStatus Field(const char *csv, size_t length, size_t *cursor,
    char *out, size_t capacity, bool *rowEnd)
{
    size_t used = 0U;
    bool quoted = *cursor < length && csv[*cursor] == '"';
    bool closed = !quoted;
    if (quoted) ++*cursor;
    while (*cursor < length) {
        char ch = csv[*cursor];
        if (quoted) {
            if (ch == '"') {
                ++*cursor;
                if (*cursor < length && csv[*cursor] == '"') { ch = '"'; ++*cursor; }
                else { closed = true; break; }
            } else ++*cursor;
        } else {
            if (ch == ',' || ch == '\n' || ch == '\r') break;
            if (ch == '"') return UMI_STATUS_PARSE_ERROR;
            ++*cursor;
        }
        if (used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        out[used++] = ch;
    }
    out[used] = '\0';
    if (!closed) return UMI_STATUS_PARSE_ERROR;
    if (*cursor == length) { *rowEnd = true; return UMI_STATUS_OK; }
    if (csv[*cursor] == ',') { ++*cursor; *rowEnd = false; return UMI_STATUS_OK; }
    if (csv[*cursor] == '\n') { ++*cursor; *rowEnd = true; return UMI_STATUS_OK; }
    if (csv[*cursor] == '\r' && length - *cursor >= 2U && csv[*cursor + 1U] == '\n') {
        *cursor += 2U; *rowEnd = true; return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}
static bool Quantity(const char *text, uint64_t *out)
{
    uint64_t value = 0U;
    if (text[0] == '\0') return false;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        unsigned char ch = (unsigned char)text[i];
        if (ch < '0' || ch > '9' || value > ((uint64_t)INT64_MAX - (ch - '0')) / 10U) return false;
        value = value * 10U + (ch - '0');
    }
    *out = value;
    return true;
}
UmiStatus EwsParse(const char *csv, size_t length, size_t recipe,
    UmiEnterpriseRow *rows, size_t *outCount, UmiEnterpriseIssue *issue)
{
    static const char *const header[] = {"item_id", "label", "quantity"};
    size_t cursor = 0U, record = 1U, count = 0U;
    if (outCount != NULL) *outCount = 0U;
    if (issue != NULL) (void)memset(issue, 0, sizeof(*issue));
    if (csv == NULL || rows == NULL || outCount == NULL || recipe >= UMI_ENTERPRISE_RECIPE_COUNT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length == 0U || length >= UMI_ENTERPRISE_CSV_CAPACITY)
        return Fail(issue, 0U, 0U, UMI_STATUS_CAPACITY_EXCEEDED, "Supply between 1 and 16384 UTF-8 bytes.");
    if (!EwsUtf8(csv, length, true))
        return Fail(issue, 0U, 0U, UMI_STATUS_PARSE_ERROR, "Input contains NUL, invalid UTF-8 or unsupported controls.");
    /* A UTF-8 BOM is accepted only before the first header field. */
    if (length >= 3U && memcmp(csv, "\xef\xbb\xbf", 3U) == 0) cursor = 3U;
    while (cursor < length) {
        char fields[3][UMI_ENTERPRISE_LABEL_CAPACITY];
        UmiEnterpriseRow row = {0};
        for (size_t column = 0U; column < 3U; ++column) {
            bool end = false;
            UmiStatus status = Field(csv, length, &cursor, fields[column], sizeof(fields[column]), &end);
            if (status != UMI_STATUS_OK)
                return Fail(issue, record, column + 1U, status, "Invalid CSV quoting, newline or field length.");
            if (end != (column == 2U))
                return Fail(issue, record, column + 1U, UMI_STATUS_PARSE_ERROR, "Every record needs exactly three fields.");
        }
        if (record == 1U) {
            for (size_t i = 0U; i < 3U; ++i)
                if (strcmp(fields[i], header[i]) != 0)
                    return Fail(issue, record, i + 1U, UMI_STATUS_PARSE_ERROR, "Use the exact header: item_id,label,quantity.");
        } else {
            if (count >= UMI_ENTERPRISE_MAX_IMPORT_ROWS)
                return Fail(issue, record, 0U, UMI_STATUS_CAPACITY_EXCEEDED, "An import accepts at most 32 data records.");
            if (!EwsId(fields[0]))
                return Fail(issue, record, 1U, UMI_STATUS_INVALID_ARGUMENT, "Item ID needs 1-63 ASCII letters, digits, dots, underscores or hyphens; start with a letter or digit.");
            if (!EwsText(fields[1], sizeof(fields[1]), false, true))
                return Fail(issue, record, 2U, UMI_STATUS_INVALID_ARGUMENT, "A label is required and must fit in 191 UTF-8 bytes.");
            if (!Quantity(fields[2], &row.quantity))
                return Fail(issue, record, 3U, UMI_STATUS_INVALID_ARGUMENT, "Quantity must contain decimal digits only and fit in 0..9223372036854775807.");
            for (size_t i = 0U; i < count; ++i)
                if (strcmp(rows[i].id, fields[0]) == 0)
                    return Fail(issue, record, 1U, UMI_STATUS_ALREADY_EXISTS, "An item ID occurs twice in this import.");
            (void)EwsCopy(row.id, sizeof(row.id), fields[0]);
            (void)EwsCopy(row.label, sizeof(row.label), fields[1]);
            if (recipe == 1U) {
                for (size_t i = 0U; row.label[i] != '\0'; ++i)
                    if (row.label[i] >= 'a' && row.label[i] <= 'z') row.label[i] = (char)(row.label[i] - 'a' + 'A');
            }
            rows[count++] = row;
        }
        ++record;
    }
    if (record == 1U || count == 0U)
        return Fail(issue, record, 0U, UMI_STATUS_INVALID_ARGUMENT, "Supply the header and at least one data record.");
    *outCount = count;
    return UMI_STATUS_OK;
}
UmiStatus EwsPreview(const EwsState *state, const char *datasetId,
    const char *recipeId, const char *csv, size_t length,
    UmiEnterprisePreview *outPreview, UmiEnterpriseIssue *issue)
{
    UmiEnterpriseRow rows[UMI_ENTERPRISE_MAX_IMPORT_ROWS];
    UmiEnterprisePreview *preview;
    const EwsDataset *dataset;
    size_t datasetIndex, recipe, count = 0U;
    UmiStatus status;
    if (outPreview == NULL || state == NULL || !EwsId(datasetId)) return UMI_STATUS_INVALID_ARGUMENT;
    if (issue != NULL) (void)memset(issue, 0, sizeof(*issue));
    datasetIndex = EwsDatasetIndex(state, datasetId); recipe = EwsRecipeIndex(recipeId);
    if (datasetIndex == SIZE_MAX || recipe == SIZE_MAX) return Fail(issue, 0U, 0U, UMI_STATUS_NOT_FOUND, "Dataset or built-in recipe was not found.");
    if (!state->recipes[recipe]) return Fail(issue, 0U, 0U, UMI_STATUS_PERMISSION_DENIED, "Enable this built-in recipe in Marketplace before preparing imports.");
    status = EwsParse(csv, length, recipe, rows, &count, issue);
    if (status != UMI_STATUS_OK) return status;
    preview = calloc(1U, sizeof(*preview));
    if (preview == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    dataset = &state->datasets[datasetIndex];
    (void)EwsCopy(preview->datasetId, sizeof(preview->datasetId), datasetId);
    (void)EwsCopy(preview->recipeId, sizeof(preview->recipeId), recipeId);
    preview->datasetGeneration = dataset->info.generation;
    preview->workspaceRevision = state->revision; preview->rowCount = count;
    for (size_t i = 0U; i < count; ++i) {
        UmiEnterpriseChange *change = &preview->changes[i];
        size_t found = SIZE_MAX;
        change->after = rows[i];
        for (size_t j = 0U; j < dataset->info.rowCount; ++j)
            if (strcmp(rows[i].id, dataset->rows[j].id) == 0) { found = j; break; }
        if (found == SIZE_MAX) { change->kind = UMI_ENTERPRISE_INSERT; ++preview->insertCount; }
        else {
            change->before = dataset->rows[found];
            if (rows[i].quantity == change->before.quantity && strcmp(rows[i].label, change->before.label) == 0) {
                change->kind = UMI_ENTERPRISE_UNCHANGED; ++preview->unchangedCount;
            } else { change->kind = UMI_ENTERPRISE_UPDATE; ++preview->updateCount; }
        }
    }
    if (preview->insertCount > UMI_ENTERPRISE_MAX_ROWS - dataset->info.rowCount)
        status = Fail(issue, 0U, 0U, UMI_STATUS_CAPACITY_EXCEEDED, "The resulting dataset would exceed 64 rows.");
    else *outPreview = *preview;
    free(preview);
    return status;
}
