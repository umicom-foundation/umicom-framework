/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/value.c
 *
 * PURPOSE:
 *   Implement owned UI values with deterministic copying and comparison.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/value.h"
#include <string.h>
#include "ui_internal.h"


void umi_ui_value_clear(UmiUiValue *value)
{
    if (value != NULL) (void)memset(value, 0, sizeof(*value));
}
UmiStatus umi_ui_value_set_boolean(UmiUiValue *value, int enabled)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_BOOLEAN;
    value->boolean_value = enabled != 0; return UMI_STATUS_OK;
}
UmiStatus umi_ui_value_set_integer(UmiUiValue *value, int64_t number)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_INTEGER;
    value->integer_value = number; return UMI_STATUS_OK;
}
UmiStatus umi_ui_value_set_real(UmiUiValue *value, double number)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_REAL;
    value->real_value = number; return UMI_STATUS_OK;
}
UmiStatus umi_ui_value_set_string(UmiUiValue *value, const char *text)
{
    if (value == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_STRING;
    return umi_ui_copy_text(value->string_value, sizeof(value->string_value), text)
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
UmiStatus umi_ui_value_copy(UmiUiValue *destination, const UmiUiValue *source)
{
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source; return UMI_STATUS_OK;
}
int umi_ui_value_equal(const UmiUiValue *left, const UmiUiValue *right)
{
    if (left == NULL || right == NULL || left->kind != right->kind) return 0;
    switch (left->kind) {
        case UMI_UI_VALUE_BOOLEAN: return left->boolean_value == right->boolean_value;
        case UMI_UI_VALUE_INTEGER: return left->integer_value == right->integer_value;
        case UMI_UI_VALUE_REAL: return left->real_value == right->real_value;
        case UMI_UI_VALUE_STRING: return strcmp(left->string_value, right->string_value) == 0;
        default: return 1;
    }
}
