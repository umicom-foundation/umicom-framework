/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/uat_database_rows.c
 *
 * PURPOSE:
 *   Admit bounded database snapshots and compare exact typed query results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/uat_database_rows.h"

#include "umicom/base/utf8.h"
#include <math.h>
#include <string.h>

/* Reject unsupported native storage kinds and malformed spans before
 * comparing values. A fixture should never report a pass from partial data. */
static UmiStatus ValidateCell(const UmiUatDatabaseCell *cell)
{
    switch (cell->kind) {
    case UMI_DATA_VALUE_NULL:
    case UMI_DATA_VALUE_INTEGER:
        return UMI_STATUS_OK;
    case UMI_DATA_VALUE_REAL:
        return isfinite(cell->value.real) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
    case UMI_DATA_VALUE_TEXT:
    case UMI_DATA_VALUE_BLOB: {
        const UmiUatDatabaseBytes bytes = cell->value.bytes;
        if (bytes.length > UMI_UAT_DATABASE_VALUE_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (bytes.length == 0U) return UMI_STATUS_OK;
        if (bytes.data == NULL) return UMI_STATUS_INVALID_ARGUMENT;
        if (cell->kind == UMI_DATA_VALUE_TEXT && !UmiUtf8Validate(bytes.data, bytes.length, NULL))
            return UMI_STATUS_PARSE_ERROR;
        return UMI_STATUS_OK;
    }
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
}

/* Bounds precede multiplication and pointer access. Keeping cell_count
 * explicit prevents a caller from presenting a short row buffer as complete. */
static UmiStatus ValidateRows(const UmiUatDatabaseRows *rows)
{
    if (rows == NULL || rows->column_count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (rows->row_count > UMI_UAT_DATABASE_MAX_ROWS ||
        rows->column_count > UMI_UAT_DATABASE_MAX_COLUMNS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (rows->cell_count != rows->row_count * rows->column_count ||
        (rows->cell_count != 0U && rows->cells == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < rows->cell_count; ++i) {
        UmiStatus status = ValidateCell(&rows->cells[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/* Call only after matching storage kinds. SQL driver conversions before
 * comparison would otherwise hide a numeric or binary representation change. */
static bool SameValue(const UmiUatDatabaseCell *left, const UmiUatDatabaseCell *right)
{
    switch (left->kind) {
    case UMI_DATA_VALUE_NULL:
        return true;
    case UMI_DATA_VALUE_INTEGER:
        return left->value.integer == right->value.integer;
    case UMI_DATA_VALUE_REAL:
        return left->value.real == right->value.real;
    case UMI_DATA_VALUE_TEXT:
    case UMI_DATA_VALUE_BLOB:
        return left->value.bytes.length == right->value.bytes.length &&
            (left->value.bytes.length == 0U || memcmp(left->value.bytes.data,
                right->value.bytes.data, left->value.bytes.length) == 0);
    default:
        return false;
    }
}

UmiStatus UmiUatCompareDatabaseRows(const UmiUatDatabaseRows *expected,
    const UmiUatDatabaseRows *actual, UmiUatDatabaseComparison *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ValidateRows(expected);
    if (status == UMI_STATUS_OK) status = ValidateRows(actual);
    if (status != UMI_STATUS_OK) return status;

    /* Publish a small, deterministic finding. The caller can report the
     * failing location without adding table contents to workflow evidence. */
    UmiUatDatabaseComparison result = {UMI_UAT_DATABASE_EQUAL, SIZE_MAX, SIZE_MAX};
    if (expected->column_count != actual->column_count) {
        result.match = UMI_UAT_DATABASE_COLUMNS_DIFFER;
    } else if (expected->row_count != actual->row_count) {
        result.match = UMI_UAT_DATABASE_ROWS_DIFFER;
    } else {
        for (size_t i = 0U; i < expected->cell_count; ++i) {
            const UmiUatDatabaseCell *left = &expected->cells[i];
            const UmiUatDatabaseCell *right = &actual->cells[i];
            if (left->kind != right->kind) result.match = UMI_UAT_DATABASE_KINDS_DIFFER;
            else if (!SameValue(left, right)) result.match = UMI_UAT_DATABASE_VALUES_DIFFER;
            if (result.match != UMI_UAT_DATABASE_EQUAL) {
                result.row = i / expected->column_count;
                result.column = i % expected->column_count;
                break;
            }
        }
    }
    *out = result;
    return UMI_STATUS_OK;
}
