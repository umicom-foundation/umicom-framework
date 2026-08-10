/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/value.h
 *
 * PURPOSE:
 *   Define an owned, ABI-stable value type used by UI properties, bindings and
 *   frontend-neutral view-state snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_VALUE_H
#define UMICOM_UI_VALUE_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_VALUE_STRING_CAPACITY 512U

typedef enum UmiUiValueKind {
    UMI_UI_VALUE_NONE = 0,
    UMI_UI_VALUE_BOOLEAN = 1,
    UMI_UI_VALUE_INTEGER = 2,
    UMI_UI_VALUE_REAL = 3,
    UMI_UI_VALUE_STRING = 4
} UmiUiValueKind;

typedef struct UmiUiValue {
    UmiUiValueKind kind;
    int boolean_value;
    int64_t integer_value;
    double real_value;
    char string_value[UMI_UI_VALUE_STRING_CAPACITY];
} UmiUiValue;

void umi_ui_value_clear(UmiUiValue *value);
UmiStatus umi_ui_value_set_boolean(UmiUiValue *value, int enabled);
UmiStatus umi_ui_value_set_integer(UmiUiValue *value, int64_t number);
UmiStatus umi_ui_value_set_real(UmiUiValue *value, double number);
UmiStatus umi_ui_value_set_string(UmiUiValue *value, const char *text);
UmiStatus umi_ui_value_copy(UmiUiValue *destination, const UmiUiValue *source);
int umi_ui_value_equal(const UmiUiValue *left, const UmiUiValue *right);

#ifdef __cplusplus
}
#endif

#endif
