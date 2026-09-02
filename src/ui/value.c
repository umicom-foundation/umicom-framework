/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/value.c
 *
 * PURPOSE:
 *   Implement owned UI values with deterministic copying and comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/value.h"
#include <string.h>
#include "ui_internal.h"


/* Release or reset state held by ui value so the same storage can be reused safely. */
void umi_ui_value_clear(UmiUiValue *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value != NULL) (void)memset(value, 0, sizeof(*value));
}
/*
 * Provide the ui value set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_value_set_boolean(UmiUiValue *value, int enabled)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_BOOLEAN;
    value->boolean_value = enabled != 0; return UMI_STATUS_OK;
}
/*
 * Provide the ui value set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_value_set_integer(UmiUiValue *value, int64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_INTEGER;
    value->integer_value = number; return UMI_STATUS_OK;
}
/* Provide the ui value set real operation used by this module and its client applications. */
UmiStatus umi_ui_value_set_real(UmiUiValue *value, double number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_REAL;
    value->real_value = number; return UMI_STATUS_OK;
}
/*
 * Provide the ui value set string operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_value_set_string(UmiUiValue *value, const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_value_clear(value); value->kind = UMI_UI_VALUE_STRING;
    return umi_ui_copy_text(value->string_value, sizeof(value->string_value), text)
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
/* Copy ui value into module-owned storage so callers keep ownership of their input values. */
UmiStatus umi_ui_value_copy(UmiUiValue *destination, const UmiUiValue *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *destination = *source; return UMI_STATUS_OK;
}
/* Provide the ui value equal operation used by this module and its client applications. */
int umi_ui_value_equal(const UmiUiValue *left, const UmiUiValue *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL || left->kind != right->kind) return 0;
    /* Select the behaviour associated with the requested command or state value. */
    switch (left->kind) {
        case UMI_UI_VALUE_BOOLEAN: return left->boolean_value == right->boolean_value;
        case UMI_UI_VALUE_INTEGER: return left->integer_value == right->integer_value;
        case UMI_UI_VALUE_REAL: return left->real_value == right->real_value;
        case UMI_UI_VALUE_STRING: return strcmp(left->string_value, right->string_value) == 0;
        default: return 1;
    }
}
