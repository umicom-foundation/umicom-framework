/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/uat/test_database_rows.c
 *
 * PURPOSE:
 *   Verify typed query snapshots, exact bytes, admission and safe mismatch evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/uat_database_rows.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); return 1; \
} } while (0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    /* Compare a complete row-major grid. TEXT includes Unicode and an
     * embedded NUL. A BLOB uses valid UTF-8 bytes so typing can be tested alone. */
    static const unsigned char text[] = {'c', 0xC3U, 0xA9U, 0U, 'x'};
    static const unsigned char different[] = {'c', 0xC3U, 0xA9U, 0U, 'y'};
    static const unsigned char blob[] = {0x00U, 0x41U};
    static const unsigned char malformed[] = {0xC0U, 0x80U};

    UmiUatDatabaseCell expected_cells[] = {
        {.kind = UMI_DATA_VALUE_INTEGER, .value.integer = INT64_MIN},
        {.kind = UMI_DATA_VALUE_TEXT, .value.bytes = {text, sizeof(text)}},
        {.kind = UMI_DATA_VALUE_BLOB, .value.bytes = {blob, sizeof(blob)}},
        {.kind = UMI_DATA_VALUE_REAL, .value.real = 2.5},
        {.kind = UMI_DATA_VALUE_NULL},
        {.kind = UMI_DATA_VALUE_TEXT, .value.bytes = {NULL, 0U}},
    };
    UmiUatDatabaseCell actual_cells[sizeof expected_cells / sizeof expected_cells[0]];
    memcpy(actual_cells, expected_cells, sizeof(actual_cells));
    UmiUatDatabaseRows expected = {2U, 3U, 6U, expected_cells};
    UmiUatDatabaseRows actual = {2U, 3U, 6U, actual_cells};
    UmiUatDatabaseComparison wanted = {UMI_UAT_DATABASE_EQUAL, SIZE_MAX, SIZE_MAX};
    UmiStatus wanted_status = UMI_STATUS_OK;

    if (strcmp(mode, "empty") == 0) {
        expected = actual = (UmiUatDatabaseRows){0U, 3U, 0U, NULL};
    } else if (strcmp(mode, "columns") == 0) {
        actual = (UmiUatDatabaseRows){2U, 2U, 4U, actual_cells};
        wanted.match = UMI_UAT_DATABASE_COLUMNS_DIFFER;
    } else if (strcmp(mode, "rows") == 0) {
        actual = (UmiUatDatabaseRows){1U, 3U, 3U, actual_cells};
        wanted.match = UMI_UAT_DATABASE_ROWS_DIFFER;
    } else if (strcmp(mode, "order") == 0) {
        memcpy(actual_cells, expected_cells + 3, 3U * sizeof(actual_cells[0]));
        memcpy(actual_cells + 3, expected_cells, 3U * sizeof(actual_cells[0]));
        wanted = (UmiUatDatabaseComparison){UMI_UAT_DATABASE_KINDS_DIFFER, 0U, 0U};
    } else if (strcmp(mode, "integer") == 0) {
        actual_cells[0].value.integer = 0;
        wanted = (UmiUatDatabaseComparison){UMI_UAT_DATABASE_VALUES_DIFFER, 0U, 0U};
    } else if (strcmp(mode, "real") == 0) {
        actual_cells[3].value.real = 2.5000000000000004;
        wanted = (UmiUatDatabaseComparison){UMI_UAT_DATABASE_VALUES_DIFFER, 1U, 0U};
    } else if (strcmp(mode, "signed-zero") == 0) {
        expected_cells[3].value.real = -0.0;
        actual_cells[3].value.real = 0.0;
    } else if (strcmp(mode, "null-empty") == 0) {
        actual_cells[4] = (UmiUatDatabaseCell){.kind = UMI_DATA_VALUE_TEXT, .value.bytes = {NULL, 0U}};
        wanted = (UmiUatDatabaseComparison){UMI_UAT_DATABASE_KINDS_DIFFER, 1U, 1U};
    } else if (strcmp(mode, "embedded-nul") == 0) {
        actual_cells[1].value.bytes.data = different;
        wanted = (UmiUatDatabaseComparison){UMI_UAT_DATABASE_VALUES_DIFFER, 0U, 1U};
    } else if (strcmp(mode, "blob-type") == 0) {
        actual_cells[2].kind = UMI_DATA_VALUE_TEXT;
        wanted = (UmiUatDatabaseComparison){UMI_UAT_DATABASE_KINDS_DIFFER, 0U, 2U};
    } else if (strcmp(mode, "malformed-text") == 0) {
        /* A mismatch in an earlier value cannot conceal later invalid UTF-8. */
        actual_cells[0].value.integer = 0;
        actual_cells[1].value.bytes = (UmiUatDatabaseBytes){malformed, sizeof(malformed)};
        wanted_status = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(mode, "oversized-cell") == 0) {
        actual_cells[2].value.bytes.length = UMI_UAT_DATABASE_VALUE_MAX_BYTES + 1U;
        wanted_status = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "unsupported-real") == 0) {
        actual_cells[3].value.real = NAN;
        wanted_status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "invalid-kind") == 0) {
        actual_cells[4].kind = (UmiDataValueKind)255;
        wanted_status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "short-grid") == 0) {
        --actual.cell_count;
        wanted_status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "null-storage") == 0) {
        actual_cells[1].value.bytes.data = NULL;
        wanted_status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "count-limit") == 0) {
        actual.row_count = SIZE_MAX;
        wanted_status = UMI_STATUS_CAPACITY_EXCEEDED;
    } else {
        CHECK(strcmp(mode, "exact") == 0);
    }

    /* Failed admission must preserve all fields of the previous observation. */
    const UmiUatDatabaseComparison prior = {UMI_UAT_DATABASE_ROWS_DIFFER, 25U, 19U};
    UmiUatDatabaseComparison result = prior;
    CHECK(UmiUatCompareDatabaseRows(&expected, &actual, &result) == wanted_status);
    if (wanted_status != UMI_STATUS_OK) wanted = prior;
    CHECK(result.match == wanted.match && result.row == wanted.row && result.column == wanted.column);
    return 0;
}
