/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/row.c
 *
 * PURPOSE:
 *   Implement bounded named typed values with replacement, owned text and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/row.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/data_internal.h"

typedef struct UmiDataColumn {
    char name[128];
    UmiDataValue value;
} UmiDataColumn;

struct UmiDataRow {
    UmiDataColumn *columns;
    size_t capacity;
    size_t count;
};

UmiStatus umi_data_row_create(size_t capacity, UmiDataRow **out_row)
{
    UmiDataRow *row;
    if (capacity == 0U || out_row == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_row = NULL;
    row = (UmiDataRow *)calloc(1U, sizeof(*row));
    if (row == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    row->columns = (UmiDataColumn *)calloc(capacity, sizeof(*row->columns));
    if (row->columns == NULL) {
        free(row);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    row->capacity = capacity;
    *out_row = row;
    return UMI_STATUS_OK;
}

void umi_data_row_destroy(UmiDataRow *row)
{
    size_t index;
    if (row == NULL) return;
    for (index = 0U; index < row->count; ++index) {
        free(row->columns[index].value.text_value);
    }
    free(row->columns);
    free(row);
}

static UmiDataColumn *find_or_add(UmiDataRow *row, const char *column)
{
    size_t index;
    for (index = 0U; index < row->count; ++index) {
        if (strcmp(row->columns[index].name, column) == 0) {
            free(row->columns[index].value.text_value);
            row->columns[index].value.text_value = NULL;
            return &row->columns[index];
        }
    }
    if (row->count >= row->capacity) return NULL;
    (void)snprintf(row->columns[row->count].name,
                   sizeof(row->columns[row->count].name),
                   "%s",
                   column);
    return &row->columns[row->count++];
}

UmiStatus umi_data_row_set_integer(UmiDataRow *row,
                                   const char *column,
                                   int64_t value)
{
    UmiDataColumn *entry;
    if (row == NULL || column == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    entry = find_or_add(row, column);
    if (entry == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry->value.type = UMI_DATA_INTEGER;
    entry->value.integer_value = value;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_row_set_real(UmiDataRow *row,
                                const char *column,
                                double value)
{
    UmiDataColumn *entry;
    if (row == NULL || column == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    entry = find_or_add(row, column);
    if (entry == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry->value.type = UMI_DATA_REAL;
    entry->value.real_value = value;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_row_set_text(UmiDataRow *row,
                                const char *column,
                                const char *value)
{
    UmiDataColumn *entry;
    if (row == NULL || column == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    entry = find_or_add(row, column);
    if (entry == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry->value.text_value = umi_data_strdup(value);
    if (entry->value.text_value == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    entry->value.type = UMI_DATA_TEXT;
    return UMI_STATUS_OK;
}

const UmiDataValue *umi_data_row_get(const UmiDataRow *row,
                                     const char *column)
{
    size_t index;
    if (row == NULL || column == NULL) return NULL;
    for (index = 0U; index < row->count; ++index) {
        if (strcmp(row->columns[index].name, column) == 0) {
            return &row->columns[index].value;
        }
    }
    return NULL;
}

size_t umi_data_row_count(const UmiDataRow *row)
{
    return row != NULL ? row->count : 0U;
}
