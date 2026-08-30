/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/value.c
 *
 * PURPOSE:
 *   Implement typed context values with strict validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/value.h"
#include <math.h>
#include <string.h>
void umi_context_value_init(UmiContextValue *value, const char *name)
{
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    if (name != NULL) (void)umi_context_copy_text(value->name, sizeof(value->name), name);
}
UmiStatus umi_context_value_set_text(UmiContextValue *value, const char *text)
{
    UmiStatus status;
    if (value == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_context_copy_text(value->text, sizeof(value->text), text);
    if (status == UMI_STATUS_OK) value->kind = UMI_CONTEXT_VALUE_TEXT;
    return status;
}
UmiStatus umi_context_value_set_integer(UmiContextValue *value, int64_t number)
{
if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
value->integer_value=number;
value->kind=UMI_CONTEXT_VALUE_INTEGER;
return UMI_STATUS_OK;
}
UmiStatus umi_context_value_set_unsigned(UmiContextValue *value, uint64_t number)
{
if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
value->unsigned_value=number;
value->kind=UMI_CONTEXT_VALUE_UNSIGNED;
return UMI_STATUS_OK;
}
UmiStatus umi_context_value_set_decimal(UmiContextValue *value, double number)
{
if (value == NULL || !isfinite(number)) return UMI_STATUS_INVALID_ARGUMENT;
value->decimal_value=number;
value->kind=UMI_CONTEXT_VALUE_DECIMAL;
return UMI_STATUS_OK;
}
UmiStatus umi_context_value_set_boolean(UmiContextValue *value, bool state)
{
if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
value->boolean_value=state;
value->kind=UMI_CONTEXT_VALUE_BOOLEAN;
return UMI_STATUS_OK;
}
UmiStatus umi_context_value_validate(const UmiContextValue *value)
{
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(value->name, sizeof(value->name))) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->kind < UMI_CONTEXT_VALUE_NONE || value->kind > UMI_CONTEXT_VALUE_BOOLEAN) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->kind == UMI_CONTEXT_VALUE_TEXT && !umi_context_text_is_valid(value->text,sizeof(value->text))) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->kind == UMI_CONTEXT_VALUE_DECIMAL && !isfinite(value->decimal_value)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
bool umi_context_value_equal(const UmiContextValue *left, const UmiContextValue *right)
{
    if (left == NULL || right == NULL || left->kind != right->kind || strcmp(left->name,right->name)!=0) return false;
    switch (left->kind) {
    case UMI_CONTEXT_VALUE_NONE: return true;
    case UMI_CONTEXT_VALUE_TEXT: return strcmp(left->text,right->text)==0;
    case UMI_CONTEXT_VALUE_INTEGER: return left->integer_value==right->integer_value;
    case UMI_CONTEXT_VALUE_UNSIGNED: return left->unsigned_value==right->unsigned_value;
    case UMI_CONTEXT_VALUE_DECIMAL: return left->decimal_value==right->decimal_value;
    case UMI_CONTEXT_VALUE_BOOLEAN: return left->boolean_value==right->boolean_value;
    default: return false;
    
}
}
