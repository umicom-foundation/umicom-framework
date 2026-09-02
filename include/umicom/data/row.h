/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/row.h
 *
 * PURPOSE:
 *   Define typed row values for repository and adapter boundaries without exposing database-vendor row objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ROW_H
#define UMICOM_DATA_ROW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named data value type values accepted by this public contract.
 */
typedef enum UmiDataValueType {
    UMI_DATA_NULL = 0,
    UMI_DATA_INTEGER = 1,
    UMI_DATA_REAL = 2,
    UMI_DATA_TEXT = 3
} UmiDataValueType;

/**
 * Represent the data value data shared with callers of this public contract.
 */
typedef struct UmiDataValue {
    UmiDataValueType type;
    int64_t integer_value;
    double real_value;
    char *text_value;
} UmiDataValue;

/**
 * Represent the data row data shared with callers of this public contract.
 */
typedef struct UmiDataRow UmiDataRow;

/**
 * Initialise data row from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_data_row_create(size_t capacity, UmiDataRow **out_row);
/**
 * Release or reset state held by data row so the same storage can be reused safely.
 */
void umi_data_row_destroy(UmiDataRow *row);
/**
 * Provide the data row set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_row_set_integer(UmiDataRow *row,
                                   const char *column,
                                   int64_t value);
/**
 * Provide the data row set real operation used by this module and its client applications.
 */
UmiStatus umi_data_row_set_real(UmiDataRow *row,
                                const char *column,
                                double value);
/**
 * Provide the data row set text operation used by this module and its client applications.
 */
UmiStatus umi_data_row_set_text(UmiDataRow *row,
                                const char *column,
                                const char *value);
/**
 * Provide the data row get operation used by this module and its client applications.
 */
const UmiDataValue *umi_data_row_get(const UmiDataRow *row,
                                     const char *column);
/**
 * Return the number of records represented by data row without changing their state.
 */
size_t umi_data_row_count(const UmiDataRow *row);

#ifdef __cplusplus
}
#endif

#endif
