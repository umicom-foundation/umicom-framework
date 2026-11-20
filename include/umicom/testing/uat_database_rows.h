/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/uat_database_rows.h
 *
 * PURPOSE:
 *   Compare typed database rows exactly and report only mismatch coordinates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TESTING_UAT_DATABASE_ROWS_H
#define UMICOM_TESTING_UAT_DATABASE_ROWS_H

#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UAT_DATABASE_MAX_ROWS 64U
#define UMI_UAT_DATABASE_MAX_COLUMNS 16U
#define UMI_UAT_DATABASE_VALUE_MAX_BYTES 4096U

/* A borrowed text or binary value carries its complete length. An empty
 * value is distinct from database NULL, even when its data pointer is NULL. */
typedef struct UmiUatDatabaseBytes {
    const unsigned char *data;
    size_t length;
} UmiUatDatabaseBytes;

typedef struct UmiUatDatabaseCell {
    UmiDataValueKind kind;
    union {
        int64_t integer;
        double real;
        UmiUatDatabaseBytes bytes;
    } value;
} UmiUatDatabaseCell;

/* Supply cells in query order, row by row. Column count remains known for
 * a query returning zero rows. Buffers stay borrowed through comparison. */
typedef struct UmiUatDatabaseRows {
    size_t row_count;
    size_t column_count;
    size_t cell_count;
    const UmiUatDatabaseCell *cells;
} UmiUatDatabaseRows;

typedef enum UmiUatDatabaseMatch {
    UMI_UAT_DATABASE_EQUAL = 0,
    UMI_UAT_DATABASE_COLUMNS_DIFFER,
    UMI_UAT_DATABASE_ROWS_DIFFER,
    UMI_UAT_DATABASE_KINDS_DIFFER,
    UMI_UAT_DATABASE_VALUES_DIFFER
} UmiUatDatabaseMatch;

typedef struct UmiUatDatabaseComparison {
    UmiUatDatabaseMatch match;
    size_t row;
    size_t column;
} UmiUatDatabaseComparison;

/* Admit both complete result sets before comparing. Return OK with the
 * first mismatch, or EQUAL on success. Row and column are zero-based and
 * are SIZE_MAX when the finding concerns result shape or equality.
 *
 * Compare NULL, INTEGER, finite REAL, UTF-8 TEXT and BLOB exactly. REAL uses
 * numeric equality so either signed zero represents the same value. TEXT
 * compares all bytes, including any embedded NUL. Other kinds require an
 * explicit adapter representation before use here.
 *
 * On invalid input leave out unchanged. The caller supplies disjoint result
 * storage and readable spans. The service reads no database or connection
 * and never publishes actual cell contents in the result. */
UmiStatus UmiUatCompareDatabaseRows(const UmiUatDatabaseRows *expected,
    const UmiUatDatabaseRows *actual, UmiUatDatabaseComparison *out);

#ifdef __cplusplus
}
#endif
#endif
