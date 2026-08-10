/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/row.h
 *
 * PURPOSE:
 *   Define typed row values for repository and adapter boundaries without exposing database-vendor row objects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ROW_H
#define UMICOM_DATA_ROW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDataValueType {
    UMI_DATA_NULL = 0,
    UMI_DATA_INTEGER = 1,
    UMI_DATA_REAL = 2,
    UMI_DATA_TEXT = 3
} UmiDataValueType;

typedef struct UmiDataValue {
    UmiDataValueType type;
    int64_t integer_value;
    double real_value;
    char *text_value;
} UmiDataValue;

typedef struct UmiDataRow UmiDataRow;

UmiStatus umi_data_row_create(size_t capacity, UmiDataRow **out_row);
void umi_data_row_destroy(UmiDataRow *row);
UmiStatus umi_data_row_set_integer(UmiDataRow *row,
                                   const char *column,
                                   int64_t value);
UmiStatus umi_data_row_set_real(UmiDataRow *row,
                                const char *column,
                                double value);
UmiStatus umi_data_row_set_text(UmiDataRow *row,
                                const char *column,
                                const char *value);
const UmiDataValue *umi_data_row_get(const UmiDataRow *row,
                                     const char *column);
size_t umi_data_row_count(const UmiDataRow *row);

#ifdef __cplusplus
}
#endif

#endif
