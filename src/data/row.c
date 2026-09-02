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

/*
 * Initialise data row from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_data_row_create(size_t capacity, UmiDataRow **out_row)
{
    UmiDataRow *row;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capacity == 0U || out_row == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_row = NULL;
    row = (UmiDataRow *)calloc(1U, sizeof(*row));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (row == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    row->columns = (UmiDataColumn *)calloc(capacity, sizeof(*row->columns));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (row->columns == NULL) {
        free(row);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    row->capacity = capacity;
    *out_row = row;
    return UMI_STATUS_OK;
}

/* Release or reset state held by data row so the same storage can be reused safely. */
void umi_data_row_destroy(UmiDataRow *row)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (row == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < row->count; ++index) {
        free(row->columns[index].value.text_value);
    }
    free(row->columns);
    free(row);
}

/* Add find or only after its inputs and available capacity have been checked. */
static UmiDataColumn *find_or_add(UmiDataRow *row, const char *column)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < row->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(row->columns[index].name, column) == 0) {
            free(row->columns[index].value.text_value);
            row->columns[index].value.text_value = NULL;
            return &row->columns[index];
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (row->count >= row->capacity) return NULL;
    (void)snprintf(row->columns[row->count].name,
                   sizeof(row->columns[row->count].name),
                   "%s",
                   column);
    return &row->columns[row->count++];
}

/*
 * Provide the data row set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_row_set_integer(UmiDataRow *row,
                                   const char *column,
                                   int64_t value)
{
    UmiDataColumn *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (row == NULL || column == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    entry = find_or_add(row, column);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry->value.type = UMI_DATA_INTEGER;
    entry->value.integer_value = value;
    return UMI_STATUS_OK;
}

/* Provide the data row set real operation used by this module and its client applications. */
UmiStatus umi_data_row_set_real(UmiDataRow *row,
                                const char *column,
                                double value)
{
    UmiDataColumn *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (row == NULL || column == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    entry = find_or_add(row, column);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry->value.type = UMI_DATA_REAL;
    entry->value.real_value = value;
    return UMI_STATUS_OK;
}

/* Provide the data row set text operation used by this module and its client applications. */
UmiStatus umi_data_row_set_text(UmiDataRow *row,
                                const char *column,
                                const char *value)
{
    UmiDataColumn *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (row == NULL || column == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    entry = find_or_add(row, column);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry->value.text_value = umi_data_strdup(value);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry->value.text_value == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    entry->value.type = UMI_DATA_TEXT;
    return UMI_STATUS_OK;
}

/* Provide the data row get operation used by this module and its client applications. */
const UmiDataValue *umi_data_row_get(const UmiDataRow *row,
                                     const char *column)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (row == NULL || column == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < row->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(row->columns[index].name, column) == 0) {
            return &row->columns[index].value;
        }
    }
    return NULL;
}

/* Return the number of records represented by data row without changing their state. */
size_t umi_data_row_count(const UmiDataRow *row)
{
    return row != NULL ? row->count : 0U;
}
